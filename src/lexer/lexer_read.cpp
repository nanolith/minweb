/**
 * \file lexer/lexer_read.cpp
 *
 * \brief Read a token.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#include <istream>
#include <minweb/lexer.h>

using namespace minweb;
using namespace std;

/**
 * \brief Read a token from the stream.
 *
 * \returns the token read.
 */
token minweb::lexer::read()
{
    int ch = read_char();

    switch (ch)
    {
        case EOF:
            tokenbuf.clear();
            return MINWEB_TOKEN_EOF;

        case '<':
            start(ch);
            return maybeReadMacroStart();

        case '%':
            start(ch);
            return maybeReadTextSubstitution();

        case '>':
            start(ch);
            return maybeReadMacroEnd();

        case '#':
            start(ch);
            return maybeReadSpecialDirective();

        default:
            start(ch);
            return MINWEB_TOKEN_PASSTHROUGH;
    }
}

token minweb::lexer::maybeReadMacroEnd()
{
    /* match a second gt. */
    int ch = read_char();
    if (ch != '>')
        goto fail;
    accept(ch);

    /* match an at sign. */
    ch = read_char();
    if (ch != '@')
        goto fail;
    accept(ch);

    /* match the first lt. */
    ch = read_char();
    if (ch != '<')
        goto fail;
    accept(ch);

    /* match the second lt. */
    ch = read_char();
    if (ch != '<')
        goto fail;
    accept(ch);

    /* success. */
    return MINWEB_TOKEN_MACRO_END;

fail:
    if (ch != EOF)
        accept(ch);

    return MINWEB_TOKEN_PASSTHROUGH;
}

token minweb::lexer::maybeReadMacroStart()
{
    /* match a second lt. */
    int ch = read_char();
    if (ch != '<')
        goto fail;
    accept(ch);

    /* match at least one non-gt. */
    ch = read_char();
    if (ch == EOF || ch == '>')
        goto fail;
    accept(ch);

    /* while not a '>', read... */
    ch = read_char();
    while (ch != EOF && ch != '>' && ch != '\n')
    {
        accept(ch);
        ch = read_char();
    }

    /* handle end-of-input edge case. */
    if (ch == EOF || ch == '\n')
        goto fail;

    /* accept the first gt. */
    accept(ch);

    /* read the second gt. */
    ch = read_char();
    if (ch != '>')
        goto fail;

    /* accept the second gt. */
    accept(ch);

    /* read the next character. */
    ch = read_char();
    switch (ch)
    {
        case '=':
            accept(ch);
            return MINWEB_TOKEN_MACRO_START;

        case EOF:
            return MINWEB_TOKEN_MACRO_REF;

        default:
            put_back(ch);
            return MINWEB_TOKEN_MACRO_REF;
    }

fail:
    if (ch != EOF)
        accept(ch);

    return MINWEB_TOKEN_PASSTHROUGH;
}

token minweb::lexer::maybeReadTextSubstitution()
{
    /* match an open bracket. */
    int ch = read_char();
    if (ch != '[')
        goto fail;
    accept(ch);

    /* match at least one non-close bracket. */
    ch = read_char();
    if (ch == EOF || ch == ']')
        goto fail;
    accept(ch);

    /* while not a ']', read... */
    ch = read_char();
    while (ch != EOF && ch != ']')
    {
        accept(ch);
        ch = read_char();
    }

    /* handle end-of-input edge case. */
    if (ch == EOF)
        goto fail;

    /* accept the close bracket. */
    accept(ch);

    /* read the percent. */
    ch = read_char();
    if (ch != '%')
        goto fail;

    /* accept the percent. */
    accept(ch);

    /* success. */
    return MINWEB_TOKEN_TEXT_SUBSTITUTION;

fail:
    if (ch != EOF)
        accept(ch);

    return MINWEB_TOKEN_PASSTHROUGH;
}

token minweb::lexer::maybeReadSpecialDirective()
{
    /* match an open bracket. */
    int ch = read_char();
    if (ch != '[')
        goto fail;
    accept(ch);

    /* match at least one non-equals sign. */
    ch = read_char();
    if (ch == EOF || ch == '=')
        goto fail;
    accept(ch);

    /* while not a '=', read... */
    ch = read_char();
    while (ch != EOF && ch != '=')
    {
        accept(ch);
        ch = read_char();
    }

    /* handle end-of-input edge case. */
    if (ch == EOF)
        goto fail;

    /* accept the equals sign. */
    accept(ch);

    /* match at least one non-close-bracket. */
    ch = read_char();
    if (ch == EOF || ch == ']')
        goto fail;
    accept(ch);

    /* while not a ']', read... */
    ch = read_char();
    while (ch != EOF && ch != ']')
    {
        accept(ch);
        ch = read_char();
    }

    /* handle end-of-input edge case. */
    if (ch == EOF)
        goto fail;

    /* accept the close bracket. */
    accept(ch);

    /* success. */
    return MINWEB_TOKEN_SPECIAL_DIRECTIVE;

fail:
    if (ch != EOF)
        accept(ch);

    return MINWEB_TOKEN_PASSTHROUGH;
}
