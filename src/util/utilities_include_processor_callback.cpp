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
#include <sys/stat.h>

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
 * \param includes      The list of include paths to try when searching
 *                      for a particular include.
 * \param input_stack   The stack of include files.
 * \param prev          The previous special directive callback. This
 *                      will be called after this callback completes.
 *
 * \returns a callback function that enables include file processing.
 */
function<
    void (const pair<directive_type, string>&)>
minweb::utilities::include_processor_callback(
    shared_ptr<processor>* p, const list<string>& includes,
    stack<shared_ptr<pair<shared_ptr<ifstream>, string>>>& input_stack,
    function<void (const pair<directive_type, string>&)> prev)
{
    return
        [=,&input_stack](const pair<directive_type, string>& d) {
            if (MINWEB_DIRECTIVE_TYPE_INCLUDE == d.first)
            {
                bool found_include = false;

                for (auto include_path : includes)
                {
                    struct stat st;

                    /* compute the pathname. */
                    string pathname = include_path + "/" + d.second;
                    /* try to stat this file. */
                    if (0 == stat(pathname.c_str(), &st))
                    {
                        /* open the include file for reading. */
                        auto stream = make_shared<ifstream>(pathname);
                        if (!stream->good())
                        {
                            stringstream error_out;
                            error_out << "error: could not open '" << pathname
                                      << "' for reading.";

                            throw processor_error(error_out.str());
                        }

                        found_include = true;

                        /* extend the scope of this stream so it is cleaned up
                         * after the processor finishes. */
                        input_stack.push(
                            make_shared<pair<shared_ptr<ifstream>, string>>(
                                stream, pathname));

                        /* Instruct the processor to start processing this
                         * stream. */
                        (*p)->include_stream(stream.get(), pathname);
                    }
                }
            }

            if (!!prev)
            {
                prev(d);
            }
    };
}
