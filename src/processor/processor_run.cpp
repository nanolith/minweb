/**
 * \file processor/processor_run.cpp
 *
 * \brief Run the processor.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>
#include <sstream>

using namespace minweb;
using namespace std;

/**
 * \brief Run the macro processor.
 *
 * \throws processor_error if an error is encountered when processing this
 * file.
 */
void minweb::processor::run()
{
    bool in_macro = false;
    stringstream failout;
    int tok;
    int line, col, endline, endcol;
    string name;

    do
    {
        tok = in.read();
        in.read_linecol(name, line, col, endline, endcol);

        switch (tok)
        {
            case MINWEB_TOKEN_EOF:
                if (in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Expected a macro end.";

                    throw processor_error(failout.str());
                }
                else if (input_stack.size() > 0)
                {
                    /* restore the previous stream. */
                    auto val = input_stack.top();
                    in.set_input_state(
                        val->input, val->name, val->line, val->col,
                        val->putback);
                    input_stack.pop();

                    /* stub a dummy token to continue the loop. */
                    tok = MINWEB_TOKEN_PASSTHROUGH;
                }
                break;

            case MINWEB_TOKEN_MACRO_START:
                if (in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Macros cannot be nested.";

                    throw processor_error(failout.str());
                }

                in_macro = true;
                if (!!macro_begin_callback)
                {
                    macro_begin_callback(
                        lexer::macro_type_from_macro_begin(
                            in.get_token_string()));
                }
                break;

            case MINWEB_TOKEN_MACRO_END:
                if (!in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Macro end with no macro begin.";

                    throw processor_error(failout.str());
                }

                in_macro = false;
                if (!!macro_end_callback)
                {
                    macro_end_callback();
                }
                break;

            case MINWEB_TOKEN_MACRO_REF:
                if (!in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Macro references can only occur in macro ";
                    failout << "bodies.";

                    throw processor_error(failout.str());
                }

                if (!!macro_ref_callback)
                {
                    macro_ref_callback(
                        lexer::decode_macro_ref(in.get_token_string()));
                }
                break;

            case MINWEB_TOKEN_TEXT_SUBSTITUTION:
                if (!!text_substitution_callback)
                {
                    text_substitution_callback(
                        lexer::substitution_type_from_text_substitution(
                            in.get_token_string()));
                }
                break;

            case MINWEB_TOKEN_PASSTHROUGH:
                if (!!passthrough_callback)
                {
                    passthrough_callback(in.get_token_string());
                }
                break;

            case MINWEB_TOKEN_SPECIAL_DIRECTIVE:
                try
                {
                    auto decoded_directive =
                        lexer::decode_special_directive(in.get_token_string());

                    if (!!special_directive_callback)
                    {
                        special_directive_callback(decoded_directive);
                    }
                }
                catch (lexer_error& e)
                {
                    throw processor_error(e.what());
                }
                break;

            default:
                throw processor_error("Unexpect token type encountered.");
        }
    } while (tok != MINWEB_TOKEN_EOF);
}
