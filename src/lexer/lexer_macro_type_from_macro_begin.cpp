/**
 * \file lexer/lexer_macro_type_from_macro_begin.cpp
 *
 * \brief Parse a macro begin token into its constituent parts. 
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>
#include <sstream>

using namespace minweb;
using namespace std;

/**
 * \brief Get the type of macro and the macro name from a macro begin
 * string.
 *
 * \param macro_begin       A begin macro string from
 *                          \ref get_token_string().
 *
 * \returns a pair of macro_type and string, representing the macro type
 * and macro name respectively.
 *
 * \throws a lexer_error if the macro could not be parsed.
 */
pair<macro_type, string>
minweb::lexer::macro_type_from_macro_begin(const string& macro_begin)
{
    string inner;
    string typestr;
    string name;
    size_t pos;

    /* the statement must be large enough to remove <<>>= */
    if (macro_begin.size() < 5)
        goto fail;

    /* verify that this looks like a macro_begin. */
    if (macro_begin[0] != '<'
     || macro_begin[1] != '<'
     || macro_begin[macro_begin.size()-3] != '>'
     || macro_begin[macro_begin.size()-2] != '>'
     || macro_begin[macro_begin.size()-1] != '=')
        goto fail;

    /* get the inner string. */
    inner = macro_begin.substr(2, macro_begin.size() - 5);

    /* is this the root type? */
    if (inner == "*")
    {
        return make_pair(MINWEB_MACRO_TYPE_ROOT, inner);
    }

    /* is there a colon? */
    pos = inner.find(':');
    if (pos == string::npos)
    {
        return make_pair(MINWEB_MACRO_TYPE_DEFAULT, inner);
    }

    /* parse the inner string. */
    typestr = inner.substr(0, pos);
    name = inner.substr(pos+1);

    /* is this a file type? */
    if (typestr == "FILE")
    {
        return make_pair(MINWEB_MACRO_TYPE_FILE, name);
    }
    else if (typestr == "SECTION")
    {
        return make_pair(MINWEB_MACRO_TYPE_SECTION, name);
    }
    /* no valid type found, so treat it as a default type. */
    else
    {
        return make_pair(MINWEB_MACRO_TYPE_DEFAULT, inner);
    }

fail:
    stringstream failout;

    failout << "Malformed macro statement '" << macro_begin << "'";
    throw lexer_error(failout.str());
}
