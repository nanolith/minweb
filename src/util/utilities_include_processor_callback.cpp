/**
 * \file util/utilities_include_processor_callback.cpp
 *
 * \brief Create a callback to add include file processing to minweb.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */

#include <fstream>
#include <minweb/utilities.h>
#include <sstream>

using namespace minweb;
using namespace std;

/**
 * \brief The include processor callback hooks the special directive
 * processor callback, providing include file resolution.
 *
 * This requires an include path override, a shared pointer reference to
 * the processor, and the previous special directive processor, which
 * can be the empty function.
 *
 * \param p             Reference to the shared pointer populated later
 *                      with the processor instance.
 * \param include_path  The path to prepend to include file references.
 * \param input_stack   The stack of include files.
 * \param prev          The previous special directive callback. This
 *                      will be called after this callback completes.
 *
 * \returns a callback function that enables include file processing.
 */
function<
    void (const pair<directive_type, string>&)>
minweb::utilities::include_processor_callback(
    shared_ptr<processor>* p, const string& include_path,
    stack<shared_ptr<pair<shared_ptr<ifstream>, string>>>& input_stack,
    function<void (const pair<directive_type, string>&)> prev)
{
    return
        [=,&input_stack](const pair<directive_type, string>& d) {
            if (MINWEB_DIRECTIVE_TYPE_INCLUDE == d.first)
            {
                /* open the include file for reading. */
                string pathname = include_path + "/" + d.second;
                auto stream = make_shared<ifstream>(pathname);
                if (!stream->good())
                {
                    stringstream error_out;
                    error_out << "error: could not open '" << pathname
                              << "' for reading.";

                    throw processor_error(error_out.str());
                }

                /* extend the scope of this stream so it is cleaned up after the 
                 * processor finishes.
                 */
                input_stack.push(
                    make_shared<pair<shared_ptr<ifstream>, string>>(
                        stream, pathname));

                /* Instruct the processor to start processing this stream. */
                (*p)->include_stream(stream.get(), pathname);
            }

            if (!!prev)
            {
                prev(d);
            }
    };
}
