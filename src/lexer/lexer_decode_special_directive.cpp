/**
 * \file lexer/decode_special_directive.cpp
 *
 * \brief Decode a special directive.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>
#include <sstream>

using namespace minweb;
using namespace std;

/**
 * \brief Decode a special directive from a special directive token string.
 *
 * \param directive_string  The token string to decode.
 *
 * \returns the decoded special directive.
 *
 * \throws a lexer_error if the special directive could not be decoded.
 */
pair<directive_type, string>
minweb::lexer::decode_special_directive(const string& directive_string)
{
    const char* failmsg = "Malformed special directive '";
    string directive;
    string value;
    string::size_type equals_loc;

    /* the statement must be large enough to remove #[=] */
    if (directive_string.size() < 4)
        goto fail;

    equals_loc = directive_string.find('=');

    /* verify that this looks like a special directive. */
    if (directive_string[0] != '#'
     || directive_string[1] != '['
     || directive_string[directive_string.size()-1] != ']'
     || equals_loc == string::npos)
        goto fail;

    /* decode the directive. */
    directive = directive_string.substr(2, equals_loc - 2);
    value =
         directive_string.substr(
            equals_loc+1,
            directive_string.size() - equals_loc - 2);

    if (directive == "include")
    {
        return make_pair(MINWEB_DIRECTIVE_TYPE_INCLUDE, value);
    }
    else if (directive == "language")
    {
        return make_pair(MINWEB_DIRECTIVE_TYPE_LANGUAGE, value);
    }
    else
    {
        failmsg = "Unsupported directive type '";
        goto fail;
    }

fail:
    stringstream failout;

    failout << failmsg << directive_string << "'";
    throw lexer_error(failout.str());
}
