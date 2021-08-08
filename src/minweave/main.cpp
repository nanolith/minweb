/**
 * \file minweave/main.cpp
 *
 * \brief Main entry point for the minweave tool.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <minweb/processor.h>
#include <sstream>
#include <unistd.h>

using namespace minweb;
using namespace std;

static int weave(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> source_language);

int main(int argc, char* argv[])
{
    int ch;
    shared_ptr<string> output_file;
    shared_ptr<string> source_language;

    /* reset the option indicator. */
    optind = 0;
#ifdef __OpenBSD__
    optreset = 1;
#endif

    /* parse command-line options. */
    while ((ch = getopt(argc, argv, "o:L:")) != -1)
    {
        switch (ch)
        {
            /* specify the output file. */
            case 'o':
                output_file = make_shared<string>(optarg);
                break;

            /* specify source language. */
            case 'L':
                source_language = make_shared<string>(optarg);
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

    /* run the weave command. */
    return weave(argv[0], output_file, source_language);
}

typedef map<string, shared_ptr<stringstream>> macro_map;
typedef map<string, string> setting_map;
typedef map<string, shared_ptr<setting_map>> section_map;

static int weave(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> source_language)
{
    ostream* out;

    /* open the input file. */
    ifstream in(input);
    if (!in.good())
    {
        cerr << "error: file '" << input << "' could not be opened." << endl;
        return 1;
    }

    /* if the output file is not set, make an output based on the input file
     * name. */
    if (!output_file)
    {
        output_file = make_shared<string>(input + ".tex");
    }

    cerr << "Writing to output '" << *output_file << "'" << endl;

    /* open output file. */
    ofstream outfile(*output_file);
    if (!outfile.good())
    {
        cerr << "error: file '" << *output_file << "' could not be opened."
             << endl;
        return 1;
    }

    /* "globals" for weaver callbacks. */
    out = &outfile;
    string current_section = "global";
    string macro_name = "";
    macro_map macros;
    section_map sections;

    /* handle preamble. */
    (*out) << "\\lstset{" << endl
           << "    escapeinside={(*@}{@*)}";
    if (!!source_language)
    {
        (*out) << "," << endl;
        (*out) << "    language=" << *source_language << endl;
    }
    else
    {
        (*out) << endl;
    }
    (*out) << "}" << endl << endl;

    /* write passthrough data. */
    auto passthrough_callback = [&](const string& s) {
        (*out) << s;
    };

    /* handle the beginning of macros. */
    auto macro_begin_callback = [&](const pair<macro_type, string>& m) {
        auto f = macros.find(m.second);
        /* if this macro doesn't exist, create it. */
        if (f == macros.end())
        {
            bool ignore;
            tie(f, ignore) =
                macros.insert(
                    make_pair(m.second, make_shared<stringstream>()));
        }
        /* reset for another macro block. */
        else
        {
            f->second = make_shared<stringstream>();
        }

        out = f->second.get();
        macro_name = m.second;
    };

    /* emit the macro after it has been processed. */
    auto macro_end_callback = [&]() {
        out = &outfile;

        (*out) << "\\begin{lstlisting}" << endl
               << "(*@\\verb`<<" << macro_name << ">>=`@*)";

        auto f = macros.find(macro_name);
        if (f != macros.end())
        {
            (*out) << f->second->str();
        }

        (*out) << "(*@\\verb`>>@<<`@*)" << endl
               << "\\end{lstlisting}";
    };

    /* write the macro references in the document. */
    auto macro_ref_callback = [&](const string& mn) {
        (*out) << "(*@\\verb`<<" << mn << ">>`@*)";
    };

    /* run the processor. */
    try
    {
        processor p(in);
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

    return 0;
}
