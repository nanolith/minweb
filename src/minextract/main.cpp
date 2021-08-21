/**
 * \file minextract/main.cpp
 *
 * \brief Main entry point for the minextract tool.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
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

static int list_sections(
    const string& input);

static int extract(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> section_name);

int main(int argc, char* argv[])
{
    int ch;
    shared_ptr<string> output_file;
    shared_ptr<string> section_name;
    bool call_list_sections = false;

    /* reset the option indicator. */
    optind = 0;
#ifdef __OpenBSD__
    optreset = 1;
#endif

    /* parse command-line options. */
    while ((ch = getopt(argc, argv, "Lo:S:")) != -1)
    {
        switch (ch)
        {
            /* specify the output file. */
            case 'o':
                output_file = make_shared<string>(optarg);
                break;

            case 'S':
                section_name = make_shared<string>(optarg);
                break;

            case 'L':
                call_list_sections = true;
                break;
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

    /* should we list all sections? */
    if (call_list_sections)
    {
        return list_sections(argv[0]);
    }

    /* verify that we have a section name. */
    if (!section_name)
    {
        cerr << "error: a section name MUST be provided with -S section_name."
             << endl;
        return 1;
    }

    return extract(argv[0], output_file, section_name);
}

static int list_sections(
    const string& input)
{
    ostream* out = &cout;

    /* open the input file. */
    ifstream in(input);
    if (!in.good())
    {
        cerr << "error: file '" << input << "' could not be opened." << endl;
        return 1;
    }

    auto macro_begin_callback = [&](const pair<macro_type, string>& m) {
        if (MINWEB_MACRO_TYPE_SECTION == m.first)
        {
            (*out) << m.second << endl;
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

    return 0;
}

static int extract(
    const string& input, shared_ptr<string> output_file,
    shared_ptr<string> section_name)
{
    ostream* out;

    /* open the input file. */
    ifstream in(input);
    if (!in.good())
    {
        cerr << "error: file '" << input << "' could not be opened." << endl;
        return 1;
    }

    /* if the output file is not set, make it based on the section name. */
    if (!output_file)
    {
        output_file = make_shared<string>(*section_name + ".input");
    }

    cout << "writing to " << *output_file << endl;

    /* open the output file. */
    ofstream outfile(*output_file);
    if (!outfile.good())
    {
        cerr << "error: file '" << *output_file << "' could not be opened."
             << endl;
        return 1;
    }

    /* assign output to the output file. */
    out = &outfile;

    /* "globals" for extract callbacks. */
    bool should_write_assignment = false;

    /* handle the beginning of a macro. */
    auto macro_begin_callback = [&](const pair<macro_type, string>& m) {
        /* do we care about this macro? */
        if ( MINWEB_MACRO_TYPE_SECTION == m.first
         &&  *section_name == m.second)
        {
            /* Yes. Write all assignments. */
            should_write_assignment = true;
        }
    };

    /* stop looking for assignments when a section macro ends. */
    auto macro_end_callback = [&]() {
        should_write_assignment = false;
    };

    /* handle substitutions. */
    auto text_substitution_callback =
    [&](const tuple<substitution_type, string, string>& sub) {
        if ( MINWEB_SUBSTITUTION_TYPE_ASSIGNMENT == get<0>(sub)
         &&  should_write_assignment)
        {
            (*out) << get<1>(sub) << "=" << get<2>(sub) << endl;
        }
    };

    /* run the processor. */
    try
    {
        processor p(&in);
        p.register_macro_begin_callback(macro_begin_callback);
        p.register_macro_end_callback(macro_end_callback);
        p.register_text_substitution_callback(text_substitution_callback);
        p.run();
    }
    catch (processor_error& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
