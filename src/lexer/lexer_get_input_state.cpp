/**
 * \file lexer/lexer_get_input_state.cpp
 *
 * \brief Get the current input state of the lexer.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * \brief Get the current input stream, line, column, and putback buffer.
 *
 * \param input             Pointer to be updated with the current input
 *                          stream, or NULL if the caller is uninterested.
 * \param input_name        Reference to receive the name of the input
 *                          stream.
 * \param line              Reference to be updated with current line in the
 *                          stream.
 * \param col               Reference to be updated with the current column
 *                          in the stream.
 * \param putback           Reference to be updated with the current putback
 *                          buffer in the stream.
 */
void minweb::lexer::get_input_state(
    std::istream** input, std::string& name, int& line, int& col,
    std::list<int>& putback) const
{
    *input = in;
    name = in_name;
    line = curline;
    col = curcol;
    copy(putbackbuf.begin(), putbackbuf.end(), back_inserter(putback));
}
