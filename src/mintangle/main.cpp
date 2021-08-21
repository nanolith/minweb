/**
 * \file mintangle/main.cpp
 *
 * \brief Main entry point for the mintangle tool.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <minweb/processor.h>
#include <set>
#include <sstream>
#include <unistd.h>

using namespace minweb;
using namespace std;

static int tangle(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> root);
static int list_files(
    const string& input);

int main(int argc, char* argv[])
{
    bool opt_list_files = false;
    int ch;
    shared_ptr<string> output_file;
    shared_ptr<string> root;

    /* reset the option indicator. */
    optind = 0;
#ifdef __OpenBSD__
    optreset = 1;
#endif

    /* parse command-line options. */
    while ((ch = getopt(argc, argv, "Lo:r:")) != -1)
    {
        switch (ch)
        {
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
        return list_files(argv[0]);
    }

    /* run the tangle command. */
    return tangle(argv[0], output_file, root);
}

typedef function<string ()> eval_fn;
typedef list<eval_fn> macro;
typedef map<string, pair<int, shared_ptr<macro>>> macro_map;

static int tangle(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> root)
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

    shared_ptr<macro> current_macro;
    macro_map macros;
    int macro_node = 0;

    /* handle passthrough data. */
    auto passthrough_callback = [&](const string& s) {
        if (!!current_macro)
        {
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

        current_macro = f->second.second;
        macro_node = f->second.first;
    };

    /* handle macro end. */
    auto macro_end_callback = [&]() {
        macro_node = 0;
        current_macro.reset();
    };

    /* add a macro reference. */
    auto macro_ref_callback = [&](const string& mn) {
        if (!current_macro)
            return;

        /* add a lambda for resolving this macro. */
        current_macro->push_back([=,&macros]() -> string {
            stringstream ss;
            auto ff = macros.find(mn);
            if (ff == macros.end())
            {
                ss << "<<" << mn << ">>";
            }
            else
            {
                for (auto i : *ff->second.second)
                {
                    ss << i();
                }
            }

            return ss.str();
        });
    };

    /* run the processor. */
    try
    {
        processor p(&in);
        p.register_passthrough_callback(passthrough_callback);
        p.register_macro_begin_callback(macro_begin_callback);
        p.register_macro_end_callback(macro_end_callback);
        p.register_macro_ref_callback(macro_ref_callback);
        p.run();
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

static int list_files(
    const string& input)
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

    /* handle macro begin. */
    auto macro_begin_callback = [&](const pair<macro_type, string>& m) {
        if (MINWEB_MACRO_TYPE_FILE == m.first)
        {
            file_sections.insert(m.second);
        }
    };

    /* run the processor. */
    try
    {
        processor p(&in);
        p.register_macro_begin_callback(macro_begin_callback);
        p.run();
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
