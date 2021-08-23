/**
 * \file minweave/main.cpp
 *
 * \brief Main entry point for the minweave tool.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */

#include <fstream>
#include <iostream>
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
static int weave(
    const string& input, const string& include_path,
    shared_ptr<string> output_file,
    shared_ptr<string> source_language,
    shared_ptr<string> document_template);

/**
 * \brief Main entry point for the minweave tool.
 *
 * \param argc      The number of command-line arguments.
 * \param argv      The command-line arguments.
 *
 * \returns zero on success and non-zero on failure.
 */
int main(int argc, char* argv[])
{
    int ch;
    shared_ptr<string> output_file;
    shared_ptr<string> source_language;
    shared_ptr<string> document_template;
    string include_path(".");

    /* reset the option indicator. */
    optind = 0;
#ifdef __OpenBSD__
    optreset = 1;
#endif

    /* parse command-line options. */
    while ((ch = getopt(argc, argv, "I:o:L:T:")) != -1)
    {
        switch (ch)
        {
            /* specify include path. */
            case 'I':
                include_path = optarg;
                break;

            /* specify the output file. */
            case 'o':
                output_file = make_shared<string>(optarg);
                break;

            /* specify source language. */
            case 'L':
                source_language = make_shared<string>(optarg);
                break;

            /* specify template. */
            case 'T':
                document_template = make_shared<string>(optarg);
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
    return
        weave(
            argv[0], include_path, output_file, source_language,
            document_template);
}

/** \brief The map of macros. */
typedef map<string, shared_ptr<stringstream>> macro_map;

/** \brief The map of settings. */
typedef map<string, string> setting_map;

/** \brief The map of sections. */
typedef map<string, shared_ptr<setting_map>> section_map;

/** \brief The map of variables. */
typedef map<string, string> variable_map;

/**
 * \brief Perform the "weave" operation.
 *
 * \param input             The name of the input file for weave.
 * \param include_path      The include path to use when resolving include
 *                          statements.
 * \param output_file       The optional output filename override.
 * \param source_language   The optional default source language override.
 * \param document_template The optional document template file to use to build
 *                          the final document.
 *
 * \returns zero on success and non-zero on failure.
 */
static int weave(
    const string& input, const string& include_path,
    shared_ptr<string> output_file,
    shared_ptr<string> source_language,
    shared_ptr<string> document_template)
{
    ostream* out;
    shared_ptr<ostream> outfile;
    shared_ptr<istream> tempin;
    ostream* preamble_out;

    /* open the input file. */
    ifstream in(input);
    if (!in.good())
    {
        cerr << "error: file '" << input << "' could not be opened." << endl;
        return 1;
    }

    /* if the document template is set, open the template input file. */
    if (!!document_template)
    {
        tempin = make_shared<ifstream>(*document_template);
        if (!tempin->good())
        {
            cerr << "error: file '" << *document_template
                 << "' could not be opened." << endl;
            return 1;
        }
    }

    /* if the output file is not set, make an output based on the input file
     * name. */
    if (!output_file)
    {
        output_file = make_shared<string>(input + ".tex");
    }

    cerr << "Writing to output '" << *output_file << "'" << endl;

    /* if we are using the document template, then set outfile to a
     * stringstream. */
    if (!!document_template)
    {
        outfile = make_shared<stringstream>();
    }
    else
    {
        /* open output file. */
        outfile = make_shared<ofstream>(*output_file);
        if (!outfile->good())
        {
            cerr << "error: file '" << *output_file << "' could not be opened."
                 << endl;
            return 1;
        }
    }

    /* "globals" for weaver callbacks. */
    out = outfile.get();
    string current_section = "global";
    string macro_name = "";
    macro_map macros;
    section_map sections;
    variable_map vars;
    macro_type current_macro_type;
    shared_ptr<processor> p;
    stack<shared_ptr<pair<shared_ptr<ifstream>, string>>> input_stack;
    shared_ptr<string> language_override;

    /* possible holder for the preamble. */
    auto template_preamble_stream = make_shared<stringstream>();

    /* if we are using a document template, write the preamble to our preamble
     * stream. */
    if (!!document_template)
    {
        preamble_out = template_preamble_stream.get();
    }
    /* otherwise, write it to output. */
    else
    {
        preamble_out = out;
    }

    /* handle preamble. */
    (*preamble_out) << "\\usepackage{xcolor}" << endl;
    (*preamble_out) << "\\lstset{" << endl
                    << "    escapeinside={(*@}{@*)}";
    if (!!source_language)
    {
        /* set the default source language. */
        (*preamble_out) << "," << endl;
        (*preamble_out) << "    language=" << *source_language << endl;
    }
    else
    {
        (*preamble_out) << endl;
    }
    (*preamble_out) << "}" << endl << endl;

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
        current_macro_type = m.first;

        /* if this is a section, then scan an output file for variable values */
        if (MINWEB_MACRO_TYPE_SECTION == current_macro_type)
        {
            auto fname = m.second + ".output";
            auto varin = ifstream(fname);
            if (!varin.good())
            {
                cerr << "Could not open " << fname << " for reading." << endl;
                exit(1);
            }

            do
            {
                string varstr;
                getline(varin, varstr);
                if (varin.eof())
                    break;

                auto pos = varstr.find('=');
                if (string::npos == pos)
                {
                    cerr << "Variable in " << fname << " malformed: " << varstr
                         << endl;
                    exit(1);
                }

                auto key = varstr.substr(0, pos);
                auto value = varstr.substr(pos+1);

                vars.insert(make_pair(key, value));

            } while(!varin.eof());
        }
    };

    /* emit the macro after it has been processed. */
    auto macro_end_callback = [&]() {
        out = outfile.get();

        if (MINWEB_MACRO_TYPE_SECTION != current_macro_type)
        {
            (*out) << "\\begin{lstlisting}";
            if (!!language_override)
            {
                (*out) << "[language=" << *language_override << "]" << endl;
                language_override.reset();
            }
            else
            {
                (*out) << endl;
            }
            (*out) << "(*@\\verb`<<" << macro_name << ">>=`@*)";
        }

        auto f = macros.find(macro_name);
        if (f != macros.end())
        {
            (*out) << f->second->str();
        }

        if (MINWEB_MACRO_TYPE_SECTION != current_macro_type)
        {
            (*out) << "(*@\\verb`>>@<<`@*)" << endl
                   << "\\end{lstlisting}";
        }

        vars.clear();
    };

    /* write the macro references in the document. */
    auto macro_ref_callback = [&](const string& mn) {
        (*out) << "(*@\\verb`<<" << mn << ">>`@*)";
    };

    /* handle text substitutions in the document. */
    auto text_substitution_callback =
    [&](const std::tuple<substitution_type, string, string>& ts) {
        if (MINWEB_MACRO_TYPE_SECTION == current_macro_type)
        {
            auto f = vars.find(get<1>(ts));
            if (vars.end() == f)
            {
                (*out) << "\\textcolor{red}{"
                       << "\\verb`\%[" << get<1>(ts) << " undefined]`"
                       << "}";
            }
            else
            {
                (*out) << f->second;
            }
        }
    };

    /* handle language overrides. */
    auto language_override_callback =
    [&](const pair<directive_type, string>& d) {
        if (MINWEB_DIRECTIVE_TYPE_LANGUAGE == d.first)
        {
            language_override = make_shared<string>(d.second);
        }
    };
    /* handle includes. */
    auto special_directive_callback =
        include_processor_callback(
            &p, include_path, input_stack, language_override_callback);

    /* run the processor. */
    try
    {
        p = make_shared<processor>(&in, input);
        p->register_passthrough_callback(passthrough_callback);
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

    /* if the document template is specified, run the processor over this
     * template. */
    if (!!document_template)
    {
        /* open output file. */
        ofstream tempout(*output_file);
        if (!tempout.good())
        {
            cerr << "error: file '" << *output_file << "' could not be opened."
                 << endl;
            return 1;
        }

        auto template_passthrough_callback = [&](const string& s) {
            tempout << s;
        };

        auto template_ref_callback = [&](const string& mn) {
            if ("*" == mn)
            {
                tempout << dynamic_cast<stringstream*>(outfile.get())->str();
            }
            else if ("preamble" == mn)
            {
                tempout << template_preamble_stream->str();
            }
        };

        try
        {
            /* close all previous include files. */
            while (!input_stack.empty())
                input_stack.pop();

            p = make_shared<processor>(tempin.get(), *document_template);
            p->register_passthrough_callback(template_passthrough_callback);
            p->register_macro_ref_callback(template_ref_callback);
            p->register_special_directive_callback(special_directive_callback);
            p->run();
        }
        catch(processor_error& e)
        {
            cerr << e.what() << endl;
            return 1;
        }
    }

    return 0;
}
