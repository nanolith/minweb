/**
 * \file lexer/decode_macro_ref.cpp
 *
 * \brief Decode a macro reference.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>
#include <sstream>

using namespace minweb;
using namespace std;

/**
 * \brief Decode a macro reference from a macro ref token string.
 *
 * \param macro_ref         A macro reference string from
 *                          \ref get_token_string().
 *
 * \returns the decoded reference.
 *
 * \throws a lexer_error if the macro reference could not be decoded.
 */
string
minweb::lexer::decode_macro_ref(const string& macro_ref)
{
    string inner;

    /* the statement must be large enough to remove <<>> */
    if (macro_ref.size() < 4)
        goto fail;

    /* verify that this looks like a macro reference. */
    if (macro_ref[0] != '<'
     || macro_ref[1] != '<'
     || macro_ref[macro_ref.size()-2] != '>'
     || macro_ref[macro_ref.size()-1] != '>')
        goto fail;

    /* decode the inner string. */
    return macro_ref.substr(2, macro_ref.size() - 4);

fail:
    stringstream failout;

    failout << "Malformed macro reference '" << macro_ref << "'";
    throw lexer_error(failout.str());
}
