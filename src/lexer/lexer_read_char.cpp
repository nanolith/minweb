/**
 * \file lexer/lexer_read_char.cpp
 *
 * \brief Read a character from the input stream and adjust lines / cols.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * \brief Read a character from the input stream or the putback buffer.
 *
 * \returns the character read.
 */
int minweb::lexer::read_char()
{
    int ch;

    if (putbackbuf.size() > 0)
    {
        ch = putbackbuf.front();
        putbackbuf.pop_front();
    }
    else
    {
        ch = in->get();
        if ('\n' == ch)
        {
            ++curline;
            curcol = 0;
        }
        else
        {
            ++curcol;
        }
    }

    return ch;
}
