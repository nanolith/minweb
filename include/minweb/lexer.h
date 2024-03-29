/**
 * \file minweb/lexer.h
 *
 * \brief Lexer for scanning minweb files.
 *
 * \copyright Copyright 2020-2021 Justin Handville. All rights reserved.
 */

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
    /** \brief end of input. */
    MINWEB_TOKEN_EOF,
    /** \brief the start of a macro. */
    MINWEB_TOKEN_MACRO_START,
    /** \brief the end of a macro. */
    MINWEB_TOKEN_MACRO_END,
    /** \brief A macro reference. */
    MINWEB_TOKEN_MACRO_REF,
    /** \brief a pass-through character. */
    MINWEB_TOKEN_PASSTHROUGH,
    /** \brief A text substitution. */
    MINWEB_TOKEN_TEXT_SUBSTITUTION,
    /** \brief A special directive. */
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
    /** \brief An include directive. */
    MINWEB_DIRECTIVE_TYPE_INCLUDE,
    /** \brief A language directive. */
    MINWEB_DIRECTIVE_TYPE_LANGUAGE,
};

/**
 * \brief Exception thrown by certain conversion routines in the lexer.
 */
class lexer_error : public std::runtime_error
{
public:

    /**
     * \brief Construct a lexer_error from an error string.
     *
     * \param what      A description of the error.
     */
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
     * \param name      The name of the input stream.
     */
    lexer(std::istream* input, const std::string& name);

    /**
     * \brief Read a token from the stream.
     *
     * \returns the token read.
     */
    token read();

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
    void read_linecol(
        std::string& name, int& start_line, int& start_col, int& end_line,
        int& end_col) const;

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
    void get_input_state(
        std::istream** input, std::string& name, int& line, int& col,
        std::list<int>& putback) const;

    /**
     * \brief Set the current input stream, line, column, and putback buffer.
     *
     * \param input             The new input stream.
     * \param input_name        The name of the input stream.
     * \param line              The new line.
     * \param col               The new column.
     * \param putback           The new putback buffer characters.
     */
    void set_input_state(
        std::istream* input, const std::string& input_name, int line, int col,
        const std::list<int>& putback);

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
    std::istream* in;
    std::string in_name;
    std::list<char> tokenbuf;
    std::list<int> putbackbuf;
    int curline;
    int curcol;
    int start_line;
    int start_col;
    int end_line;
    int end_col;

    /**
     * \brief Read a character from the input stream or the putback buffer.
     *
     * \returns the character read.
     */
    int read_char();

    /**
     * \brief Start a token.
     *
     * \param ch        The first character of the token.
     */
    void start(int ch);

    /**
     * \brief Accept a character into the current token value.
     *
     * \param ch        The character to accept.
     */
    void accept(int ch);

    /**
     * \brief Put a character back into the stream.
     *
     * \param ch        The character to put back.
     */
    void put_back(int ch);

    /**
     * \brief Try to read a macro end token.
     *
     * \returns the token value read.
     */
    token maybeReadMacroEnd();

    /**
     * \brief Try to read a macro start token.
     *
     * \returns the token value read.
     */
    token maybeReadMacroStart();

    /**
     * \brief Try to read a text substitution token.
     *
     * \returns the token value read.
     */
    token maybeReadTextSubstitution();

    /**
     * \brief Try to read a special directive token.
     *
     * \returns the token value read.
     */
    token maybeReadSpecialDirective();
};

} /* namespace minweb */
