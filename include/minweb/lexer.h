/**
 * \file minweb/lexer.h
 *
 * \brief Lexer for scanning minweb files.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */
#ifndef  MINWEB_LEXER_HEADER_GUARD
# define MINWEB_LEXER_HEADER_GUARD

/** C++ version check. */
#if !defined(__cplusplus) || __cplusplus < 201402L
# error This file requires C++14 or greater.
#endif

#include <functional>
#include <list>
#include <stdexcept>
#include <string>
#include <tuple>

namespace minweb {

/**
 * \brief Tokens supported by the lexer.
 */
enum token
{
    /* end of input. */
    MINWEB_TOKEN_EOF,
    /* the start of a macro. */
    MINWEB_TOKEN_MACRO_START,
    /* the end of a macro. */
    MINWEB_TOKEN_MACRO_END,
    /* A macro reference. */
    MINWEB_TOKEN_MACRO_REF,
    /* a pass-through character. */
    MINWEB_TOKEN_PASSTHROUGH,
    /* A text substitution. */
    MINWEB_TOKEN_TEXT_SUBSTITUTION,
    /* A special directive. */
    MINWEB_TOKEN_SPECIAL_DIRECTIVE,
};

/**
 * \brief Macro types supported by the lexer.
 */
enum macro_type
{
    /** \brief A default macro type. */
    MINWEB_MACRO_TYPE_DEFAULT,
    /** \brief A file macro type. */
    MINWEB_MACRO_TYPE_FILE,
    /** \brief A section macro type. */
    MINWEB_MACRO_TYPE_SECTION,
    /** \brief A root macro type. */
    MINWEB_MACRO_TYPE_ROOT,
};

/**
 * \brief Text substitution types supported by the lexer.
 */
enum substitution_type
{
    /** \brief A default substitution type. */
    MINWEB_SUBSTITUTION_TYPE_DEFAULT,
    /** \brief An assignment substitution type. */
    MINWEB_SUBSTITUTION_TYPE_ASSIGNMENT,
};

/**
 * \brief Directive types supported by the lexer.
 */
enum directive_type
{
    /* \brief An include directive. */
    MINWEB_DIRECTIVE_TYPE_INCLUDE,
    /* \brief A language directive. */
    MINWEB_DIRECTIVE_TYPE_LANGUAGE,
};

/**
 * \brief Exception thrown by certain conversion routines in the lexer.
 */
class lexer_error : public std::runtime_error
{
public:
    lexer_error(const std::string& what)
        : runtime_error(what)
    {
    }
};

/**
 * \brief Lexical scanner for minweb files.
 */
class lexer
{
public:

    /**
     * This constructor builds a lexer from an input stream reference.
     *
     * \param input     Input stream from which tokens are read.
     */
    lexer(std::istream& input);

    /**
     * \brief Read a token from the stream.
     *
     * \returns the token read.
     */
    token read();

    /**
     * \brief Get the line and column information for the current token.
     *
     * \param start_line        This reference is set to the start line of the
     *                          token.
     * \param start_col         This reference is set to the start column of the
     *                          token.
     * \param end_line          This reference is set to the end line of the
     *                          token.
     * \param end_col           This reference is set to the end column of the
     *                          token.
     */
    void read_linecol(
        int& start_line, int& start_col, int& end_line, int& end_col) const;

    /**
     * \brief Get the string value of the current token.
     *
     * \returns string value of the token or an empty string if inappropriate.
     */
    std::string get_token_string();

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
    static std::pair<macro_type, std::string>
    macro_type_from_macro_begin(const std::string& macro_begin);

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
    static std::tuple<substitution_type, std::string, std::string>
    substitution_type_from_text_substitution(const std::string& sub);

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
    static std::string
    decode_macro_ref(const std::string& macro_ref);

    /**
     * \brief Decode a special directive from a special directive token string.
     *
     * \param directive_string  The token string to decode.
     *
     * \returns the decoded special directive.
     *
     * \throws a lexer_error if the special directive could not be decoded.
     */
    static std::pair<directive_type, std::string>
    decode_special_directive(const std::string& directive_string);

private:
    std::istream& in;
    std::list<char> tokenbuf;
    std::list<int> putbackbuf;
    int curline;
    int curcol;
    int start_line;
    int start_col;
    int end_line;
    int end_col;

    int read_char();
    void start(int ch);
    void accept(int ch);
    void put_back(int ch);

    token matchSequence(
        const std::string& seq, std::function<token ()> onAccept,
        std::function<token ()> onFail);

    token maybeReadMacroEnd();
    token maybeReadMacroStart();
    token maybeReadTextSubstitution();
    token maybeReadSpecialDirective();
};

} /* namespace minweb */

#endif /*MINWEB_LEXER_HEADER_GUARD*/
