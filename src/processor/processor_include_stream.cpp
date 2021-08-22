/**
 * \file processor/processor_include_stream.cpp
 *
 * \brief Include a new stream into the processor.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */
#include <minweb/processor.h>
#include <sstream>

using namespace minweb;
using namespace std;

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
void minweb::processor::include_stream(istream* input, const string& name)
{
    istream* prev_input;
    string prev_name;
    int prev_line;
    int prev_col;
    list<int> prev_putback;

    /* get the previous input state. */
    in.get_input_state(
        &prev_input, prev_name, prev_line, prev_col, prev_putback);

    /* push this input state. */
    input_stack.push(
        make_shared<processor_saved_input>(
            prev_input, prev_name, prev_line, prev_col, prev_putback));

    /* set the new input state. */
    in.set_input_state(
        input, name, 1, 0, list<int>());
}
