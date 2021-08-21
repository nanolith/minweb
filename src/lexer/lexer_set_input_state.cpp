/**
 * \file lexer/lexer_set_input_state.cpp
 *
 * \brief Get the current input state of the lexer.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * \brief Set the current input stream, line, column, and putback buffer.
 *
 * \param input             The new input stream.
 * \param input_name        The name of the input stream.
 * \param line              The new line.
 * \param col               The new column.
 * \param putback           The new putback buffer characters.
 */
void minweb::lexer::set_input_state(
    std::istream* input, const std::string& input_name, int line, int col,
    const std::list<int>& putback)
{
    in = input;
    in_name = input_name;
    curline = line;
    curcol = col;
    putbackbuf.clear();
    copy(putback.begin(), putback.end(), back_inserter(putbackbuf));
}
