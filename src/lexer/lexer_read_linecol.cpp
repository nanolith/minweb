/**
 * \file lexer/lexer_read_linecol.cpp
 *
 * \brief Read token line column details.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * \brief Get the line and column information for the current token.
 *
 * \param name              This reference is set to the name of the input
 *                          stream.
 * \param start_line        This reference is set to the start line of the
 *                          token.
 * \param start_col         This reference is set to the start column of the
 *                          token.
 * \param end_line          This reference is set to the end line of the
 *                          token.
 * \param end_col           This reference is set to the end column of the
 *                          token.
 */
void minweb::lexer::read_linecol(
    std::string& name, int& sl, int& sc, int& el, int& ec) const
{
    name = in_name;
    sl = start_line;
    sc = start_col;
    el = start_line;
    ec = sc + tokenbuf.size() - 1;
}
