/**
 * \file minweb/processor.h
 *
 * \brief Pattern for running the minweb macro processor.
 *
 * \copyright Copyright 2020 Justin Handville. All rights reserved.
 */
#ifndef  MINWEB_PROCESSOR_HEADER_GUARD
# define MINWEB_PROCESSOR_HEADER_GUARD

/** C++ version check. */
#if !defined(__cplusplus) || __cplusplus < 201402L
# error This file requires C++14 or greater.
#endif

#include <memory>
#include <minweb/lexer.h>
#include <stack>

namespace minweb {

/**
 * \brief Exception thrown by the processor when there is an error.
 */
class processor_error : public std::runtime_error
{
public:
    processor_error(const std::string& what)
        : runtime_error(what)
    {
    }
};

/**
 * \brief Saved input type for the processor.
 */
struct processor_saved_input
{
public:
    processor_saved_input(
        std::istream* _input, const std::string& _name, int _line, int _col,
        const std::list<int>& _putback)
            : input(_input)
            , name(_name)
            , line(_line)
            , col(_col)
            , putback(_putback)
    {
    }

    std::istream* input;
    std::string name;
    int line;
    int col;
    std::list<int> putback;
};

/**
 * \brief Processor for minweb files.
 */
class processor
{
public:

    /**
     * This constructor builds a processor from an input stream reference.
     *
     * \param input     Input stream to process.
     * \param name      Input stream name.
     */
    processor(std::istream* input, const std::string& name);

    /**
     * \brief Register a callback for passthrough data.
     *
     * Passthrough data is data that is unchanged by the processor.  When not in
     * a macro, this data becomes part of the document.  When in a macro, this
     * data should be appended to the current macro block.
     */
    void register_passthrough_callback(
            std::function<void (const std::string&)> cb);

    /**
     * \brief Register a callback for the beginning of a macro.
     */
    void register_macro_begin_callback(
            std::function<void (const std::pair<macro_type, std::string>&)> cb);

    /**
     * \brief Register a callback for the end of a macro.
     */
    void register_macro_end_callback(
            std::function<void ()> cb);

    /**
     * \brief Register a callback for a macro reference.
     */
    void register_macro_ref_callback(
            std::function<void (const std::string&)> cb);

    /**
     * \brief Register a callback for a text substitution.
     */
    void register_text_substitution_callback(
            std::function<
                void (const std::tuple<
                                substitution_type, std::string, std::string>&)>
            cb);

    /**
     * \brief Register a callback for a special directive.
     */
    void register_special_directive_callback(
            std::function<
                void (const std::pair<directive_type, std::string>&)> cb);

    /**
     * \brief Run the macro processor.
     *
     * \throws processor_error if an error is encountered when processing this
     * file.
     */
    void run();

    /**
     * \brief Push the current input stream and name onto the processing stream
     * stack, and immediately start processing this input stream until EOF or
     * until it is pushed onto the stack.  On EOF, the previous input stream is
     * popped from the stack.
     *
     * This is used by the include special directive.
     *
     * \param input     The input stream to start processing.
     * \param name      The name of the input stream.
     */
    void include_stream(std::istream* input, const std::string& name);

private:
    lexer in;

    std::function<void (const std::string&)>
    passthrough_callback;

    std::function<void (const std::pair<macro_type, std::string>&)>
    macro_begin_callback;

    std::function<void ()>
    macro_end_callback;

    std::function<void (const std::string&)>
    macro_ref_callback;

    std::function<
        void (const std::tuple<substitution_type, std::string, std::string>&)>
    text_substitution_callback;

    std::function<
        void (const std::pair<directive_type, std::string>&)>
    special_directive_callback;

    std::stack<std::shared_ptr<processor_saved_input>> input_stack;
};

} /* namespace minweb */

#endif /*MINWEB_PROCESSOR_HEADER_GUARD*/
