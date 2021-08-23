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
        /* get the next token and the line / column information for this
         * token. */
        tok = in.read();
        in.read_linecol(name, line, col, endline, endcol);

        switch (tok)
        {
            /* Handle end of input. */
            case MINWEB_TOKEN_EOF:
                /* It's an error to end the file in the middle of a macro. */
                if (in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Expected a macro end.";

                    throw processor_error(failout.str());
                }
                /* if we are recursing on includes, pop the stack. */
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

            /* handle a macro start. */
            case MINWEB_TOKEN_MACRO_START:
                /* Nesting macros is illegal. */
                if (in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Macros cannot be nested.";

                    throw processor_error(failout.str());
                }

                /* let the callback know we've entered a macro. */
                in_macro = true;
                if (!!macro_begin_callback)
                {
                    macro_begin_callback(
                        lexer::macro_type_from_macro_begin(
                            in.get_token_string()));
                }
                break;

            /* handle a macro end. */
            case MINWEB_TOKEN_MACRO_END:
                /* It's illegal to end a macro unless we are in a macro. */
                if (!in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Macro end with no macro begin.";

                    throw processor_error(failout.str());
                }

                /* let the callback know we've left a macro. */
                in_macro = false;
                if (!!macro_end_callback)
                {
                    macro_end_callback();
                }
                break;

            /* handle a macro reference. */
            case MINWEB_TOKEN_MACRO_REF:
                /* it's illegal to have a macro reference outside of a macro. */
                if (!in_macro)
                {
                    failout << "Error in " << name << " at "
                            << line << ":" << col << ": ";
                    failout << "Macro references can only occur in macro ";
                    failout << "bodies.";

                    throw processor_error(failout.str());
                }

                /* let the callback know we've encountered a macro reference. */
                if (!!macro_ref_callback)
                {
                    macro_ref_callback(
                        lexer::decode_macro_ref(in.get_token_string()));
                }
                break;

            /* handle a text substitution. */
            case MINWEB_TOKEN_TEXT_SUBSTITUTION:
                /* let the callback know we've encountered a text sub. */
                if (!!text_substitution_callback)
                {
                    text_substitution_callback(
                        lexer::substitution_type_from_text_substitution(
                            in.get_token_string()));
                }
                break;

            /* handle passthrough data. */
            case MINWEB_TOKEN_PASSTHROUGH:
                /* let the callback know we've encountered passthrough data. */
                if (!!passthrough_callback)
                {
                    passthrough_callback(in.get_token_string());
                }
                break;

            /* handle a special directive. */
            case MINWEB_TOKEN_SPECIAL_DIRECTIVE:
                try
                {
                    /* decode the directive. */
                    auto decoded_directive =
                        lexer::decode_special_directive(in.get_token_string());

                    /* let the callback know we've encountered a directive. */
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

            /* the lexer has given us a token we don't know. */
            default:
                throw processor_error("Unexpect token type encountered.");
        }
    } while (tok != MINWEB_TOKEN_EOF);
}
