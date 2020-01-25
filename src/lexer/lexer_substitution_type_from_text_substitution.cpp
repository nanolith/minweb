/**
 * \file lexer/lexer_substitution_type_from_text_substitution.cpp
 *
 * \brief Parse a text substitution.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>
#include <sstream>

using namespace minweb;
using namespace std;

/**
 * \brief Get the type of text substitution and the secondary values from a
 * text substitution string.
 *
 * \param sub               A text substitution string from
 *                          \ref get_token_string().
 *
 * \returns a tuple of substitution type, key, and value (if applicable).
 *
 * \throws a lexer_error if the substitution could not be parsed.
 */
tuple<substitution_type, string, string>
minweb::lexer::substitution_type_from_text_substitution(const string& sub)
{
    string inner;
    string key;
    string value;
    size_t pos;

    /* the statement must be large enough to remove %[]% */
    if (sub.size() < 4)
        goto fail;

    /* verify that this looks like a text substitution. */
    if (sub[0] != '%'
     || sub[1] != '['
     || sub[sub.size()-2] != ']'
     || sub[sub.size()-1] != '%')
        goto fail;

    /* get the inner string. */
    inner = sub.substr(2, sub.size() - 4);

    /* is there an assignment? */
    pos = inner.find('=');
    if (pos == string::npos)
    {
        return make_tuple(MINWEB_SUBSTITUTION_TYPE_DEFAULT, inner, "");
    }

    /* parse the inner string. */
    key = inner.substr(0, pos);
    value = inner.substr(pos+1);

    /* return the key/value pair. */
    return make_tuple(MINWEB_SUBSTITUTION_TYPE_ASSIGNMENT, key, value);

fail:
    stringstream failout;

    failout << "Malformed text substitution '" << sub << "'";
    throw lexer_error(failout.str());
}
