/**
 * \file mintrace/main.cpp
 *
 * \brief Main entry point for the mintrace tool.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */

#include <config.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <libgen.h>
#include <map>
#include <memory>
#include <minweb/processor.h>
#include <minweb/utilities.h>
#include <sstream>
#include <unistd.h>

using namespace minweb;
using namespace utilities;
using namespace std;

/* forward declarations. */
static int trace(
    const string& input, const list<string>& includes);

/**
 * \brief Main entry point for the mintrace tool.
 *
 * \param argc      The number of command-line arguments.
 * \param argv      The command-line arguments.
 *
 * \returns zero on success and non-zero on failure.
 */
int main(int argc, char* argv[])
{
    int retval;
    int ch;
    list<string> includes;

    /* reset the option indicator. */
    optind = 0;
#ifdef __OpenBSD__
    optreset = 1;
#endif

    /* parse command-line options. */
    while ((ch = getopt(argc, argv, "I:v")) != -1)
    {
        switch (ch)
        {
            /* override the include path. */
            case 'I':
                includes.push_back(optarg);
                break;

            /* output the version number. */
            case 'v':
                cout << "mintrace version " << MINWEB_VERSION << endl;
                return 0;
        }
    }

    /* skip past command-line arguments. */
    argc -= optind;
    argv += optind;

    /* verify that one option remains. */
    if (argc != 1)
    {
        cerr << "error: expecting exactly one filename as an argument." << endl;
        return 1;
    }

    /* compute the directory name of the file. */
    const char* filedirname = dirname(argv[0]);
    if (nullptr == filedirname)
    {
        cerr << "error: could not get the directory name of " << argv[0]
             << endl;
        return 1;
    }
    else
    {
        includes.push_front(filedirname);
    }

    return trace(argv[0], includes);
}

/**
 * \brief Perform the "trace" operation.
 *
 * \param input         The name of the input file for the trace.
 * \param includes      The include path to use when resolving include
 *                      statements.
 *
 * \returns zero on success and non-zero on failure.
 */
static int trace(
    const string& input, const list<string>& includes)
{
    int indent = 0;

    /* open the input file. */
    ifstream in(input);
    if (!in.good())
    {
        cerr << "error: file '" << input << "' could not be opened." << endl;
        return 1;
    }

    /* "globals" for trace callbacks. */
    shared_ptr<processor> p;
    stack<shared_ptr<pair<shared_ptr<ifstream>, string>>> input_stack;

    /* handle the beginning of a macro. */
    auto macro_begin_callback = [&](const pair<macro_type, string>& m) {
        string type;

        switch (m.first)
        {
            case MINWEB_MACRO_TYPE_DEFAULT:
                type = "default";
                break;
            case MINWEB_MACRO_TYPE_FILE:
                type = "file";
                break;
            case MINWEB_MACRO_TYPE_SECTION:
                type = "section";
                break;
            case MINWEB_MACRO_TYPE_ROOT:
                type = "root";
                break;
            default:
                type = "unknown";
                break;
        }

        cout << setw(indent) << " "
             << setw(0) << "begin macro type " << type << " value " << m.second
             << endl;

        indent += 4;
    };

    /* handle the end of a macro. */
    auto macro_end_callback = [&]() {
        indent -= 4;

        cout << setw(indent) << " " << setw(0)
             << "end macro." << endl;
    };

    /* handle macro refs. */
    auto macro_ref_callback =
    [&](const std::string& ref) {
        cout << setw(indent) << " " << setw(0)
             << "macro ref " << ref << endl;
    };

    /* handle substitutions. */
    auto text_substitution_callback =
    [&](const tuple<substitution_type, string, string>& sub) {
        string type;

        switch (get<0>(sub))
        {
            case MINWEB_SUBSTITUTION_TYPE_DEFAULT:
                type = "default";
                break;
            case MINWEB_SUBSTITUTION_TYPE_ASSIGNMENT:
                type = "assignment";
                break;
            default:
                type = "unknown";
                break;
        }

        cout << setw(indent) << " " << setw(0)
             << "assignment type " << type << " value " << get<1>(sub)
             << " = " << get<2>(sub) << endl;
    };

    /* handle includes and special directives. */
    auto special_directive_callback =
        include_processor_callback(
            &p, includes, input_stack,
            [&](const pair<directive_type, string>& d) {
                string type;

                switch(d.first)
                {
                    case MINWEB_DIRECTIVE_TYPE_INCLUDE:
                        type = "include";
                        break;
                    case MINWEB_DIRECTIVE_TYPE_LANGUAGE:
                        type = "language";
                        break;
                    default:
                        type = "unknown";
                        break;
                }

                cout << setw(indent) << " " << setw(0)
                     << "directive type " << type << " value " << d.second
                     << endl;
            });

    /* run the processor. */
    try
    {
        p = make_shared<processor>(&in, input);
        p->register_macro_begin_callback(macro_begin_callback);
        p->register_macro_end_callback(macro_end_callback);
        p->register_macro_ref_callback(macro_ref_callback);
        p->register_text_substitution_callback(text_substitution_callback);
        p->register_special_directive_callback(special_directive_callback);
        p->run();
    }
    catch (processor_error& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
