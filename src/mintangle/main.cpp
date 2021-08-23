/**
 * \file mintangle/main.cpp
 *
 * \brief Main entry point for the mintangle tool.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <minweb/processor.h>
#include <minweb/utilities.h>
#include <set>
#include <sstream>
#include <unistd.h>

using namespace minweb;
using namespace utilities;
using namespace std;

/* forward declarations. */
static int tangle(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> root, const string& include_path);
static int list_files(
    const string& input, const string& include_path);

/**
 * \brief Main entry point for the mintangle tool.
 *
 * \param argc      The number of command-line arguments.
 * \param argv      The command-line arguments.
 *
 * \returns zero on success and non-zero on failure.
 */
int main(int argc, char* argv[])
{
    bool opt_list_files = false;
    int ch;
    string include_path(".");
    shared_ptr<string> output_file;
    shared_ptr<string> root;

    /* reset the option indicator. */
    optind = 0;
#ifdef __OpenBSD__
    optreset = 1;
#endif

    /* parse command-line options. */
    while ((ch = getopt(argc, argv, "I:Lo:r:")) != -1)
    {
        switch (ch)
        {
            /* should we override the include path? */
            case 'I':
                include_path = optarg;
                break;

            /* should we list all file sections? */
            case 'L':
                opt_list_files = true;
                break;

            /* specify the output file. */
            case 'o':
                output_file = make_shared<string>(optarg);
                break;

            /* specify the root for the macro. */
            case 'r':
                root = make_shared<string>(optarg);
                break;
        }
    }

    /* skip past command-line options. */
    argc -= optind;
    argv += optind;

    /* verify that one option remains. */
    if (argc != 1)
    {
        cerr << "error: expecting exactly one filename as an argument." << endl;
        return 1;
    }

    /* should we list files? */
    if (opt_list_files)
    {
        return list_files(argv[0], include_path);
    }

    /* run the tangle command. */
    return tangle(argv[0], output_file, root, include_path);
}

/** \brief An evaluation function for interpreting input. */
typedef function<string ()> eval_fn;

/** \brief A list of evaluation functions form a macro. */
typedef list<eval_fn> macro;

/** \brief A mapping of macro name to macro. */
typedef map<string, pair<int, shared_ptr<macro>>> macro_map;

/**
 * \brief Perform the "tangle" operation.
 *
 * \param input         The name of the input file for the tangle.
 * \param output_file   The optional output filename override.
 * \param root          The optional root node to act as a starting point for 
 *                      creating the output file.
 * \param include_path  The include path to use when resolving include
 *                      statements.
 *
 * \returns zero on success and non-zero on failure.
 */
static int tangle(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> root, const string& include_path)
{
    /* open the input file. */
    ifstream in(input);
    if (!in.good())
    {
        cerr << "error: file '" << input << "' could not be opened." << endl;
        return 1;
    }

    /* if the output file is not set, set it to the name of the root. */
    if (!output_file && !!root)
    {
        output_file = make_shared<string>(*root);
    }
    else if (!output_file && !root)
    {
        cerr << "Error: either the output file or an alternative root "
             << "must be specified." << endl;
        return 1;
    }

    cerr << "Writing to output '" << *output_file << "'" << endl;

    /* open output file. */
    ofstream outfile(*output_file);
    if (!outfile.good())
    {
        cerr << "error: file '" << *output_file << "' could not be opened."
             << endl;
    }

    /* the "globals" for the processor callbacks. */
    shared_ptr<processor> p;
    stack<shared_ptr<pair<shared_ptr<ifstream>, string>>> input_stack;
    shared_ptr<macro> current_macro;
    macro_map macros;
    int macro_node = 0;

    /* handle passthrough data. */
    auto passthrough_callback = [&](const string& s) {
        /* ignore the value unless we are in a macro. */
        if (!!current_macro)
        {
            /* Wrap the value in a functor space for the macro mapping. */
            current_macro->push_back([=]() -> string {
                return s;
            });
        }
    };

    /* handle macro begin. */
    auto macro_begin_callback = [&](const pair<macro_type, string>& m) {
        auto f = macros.find(m.second);
        /* if this macro doesn't exist, create it. */
        if (f == macros.end())
        {
            bool ignore;
            tie(f, ignore) =
                macros.insert(
                    make_pair(m.second,
                        make_pair(macros.size()+1, make_shared<macro>())));
        }

        /* set this macro as our current macro. */
        current_macro = f->second.second;
        macro_node = f->second.first;
    };

    /* handle macro end. */
    auto macro_end_callback = [&]() {
        /* release the current macro reference. */
        current_macro.reset();
        macro_node = 0;
    };

    /* add a macro reference. */
    auto macro_ref_callback = [&](const string& mn) {
        /* if we aren't in a macro, we don't need to resolve the reference. */
        if (!current_macro)
            return;

        /* add a lambda for resolving this macro, raising this macro to the
         * functor space for the current macro. */
        current_macro->push_back([=,&macros]() -> string {
            stringstream ss;
            auto ff = macros.find(mn);
            if (ff == macros.end())
            {
                /* if the macro definition is not available, keep it as a
                 * reference. */
                ss << "<<" << mn << ">>";
            }
            else
            {
                /* otherwise, evaluate the macro in the current space. */
                for (auto i : *ff->second.second)
                {
                    ss << i();
                }
            }

            /* return the evaluated macro. */
            return ss.str();
        });
    };

    /* handle includes. */
    auto special_directive_callback =
        include_processor_callback(
            &p, include_path, input_stack,
            [&](const pair<directive_type, string>& d) { });

    /* run the processor. */
    try
    {
        /* create the processor. */
        p = make_shared<processor>(&in, input);

        /* add the tangle callbacks. */
        p->register_passthrough_callback(passthrough_callback);
        p->register_macro_begin_callback(macro_begin_callback);
        p->register_macro_end_callback(macro_end_callback);
        p->register_macro_ref_callback(macro_ref_callback);
        p->register_special_directive_callback(special_directive_callback);

        /* run the processor over the input file. */
        p->run();
    }
    catch (processor_error& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    /* determine the root node. */
    string root_node;
    if (!!root)
        root_node = *root;
    else
        root_node = "*";

    /* get the root macro. */
    auto f = macros.find(root_node);
    if (f == macros.end())
    {
        cerr << "root node '" << root_node << "' not found in document."
             << endl;
        return 1;
    }

    /* build the root macro. */
    for (auto i : *f->second.second)
    {
        outfile << i();
    }

    return 0;
}

/**
 * \brief List all of the files available to extract in the input file.
 *
 * \param input         The input filename to scan.
 * \param include_path  The include path to use when resolving include
 *                      statements.
 *
 * \returns zero on success and non-zero on failure.
 */
static int list_files(
    const string& input, const string& include_path)
{
    /* open the input file. */
    ifstream in(input);
    if (!in.good())
    {
        cerr << "error: file '" << input << "' could not be opened." << endl;
        return 1;
    }

    /* "globals" for the callbacks. */
    set<string> file_sections;
    shared_ptr<processor> p;
    stack<shared_ptr<pair<shared_ptr<ifstream>, string>>> input_stack;

    /* handle macro begin. */
    auto macro_begin_callback = [&](const pair<macro_type, string>& m) {
        /* if this is a file-type macro... */
        if (MINWEB_MACRO_TYPE_FILE == m.first)
        {
            /* add this to the set of available files. */
            file_sections.insert(m.second);
        }
    };

    /* handle includes. */
    auto special_directive_callback =
        include_processor_callback(
            &p, include_path, input_stack,
            [&](const pair<directive_type, string>& d) { });

    /* run the processor. */
    try
    {
        /* create the processor instance. */
        p = make_shared<processor>(&in, input);

        /* register list specific processor callbacks. */
        p->register_macro_begin_callback(macro_begin_callback);
        p->register_special_directive_callback(special_directive_callback);

        /* run the processor over the input file. */
        p->run();
    }
    catch (processor_error& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    /* finally, output the file sections. */
    for (auto section : file_sections)
    {
        cout << section << endl;
    }

    return 0;
}
