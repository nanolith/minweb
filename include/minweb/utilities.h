/**
 * \file minweb/utilities.h
 *
 * \brief Utilities shared by minweb binaries.
 *
 * \copyright Copyright 2021 Justin Handville. All rights reserved.
 */
#ifndef  MINWEB_UTILITIES_HEADER_GUARD
# define MINWEB_UTILITIES_HEADER_GUARD

/** C++ version check. */
#if !defined(__cplusplus) || __cplusplus < 201402L
# error This file requires C++14 or greater.
#endif

#include <minweb/processor.h>

namespace minweb {

    namespace utilities {

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
        std::function<
            void (const std::pair<directive_type, std::string>&)>
        include_processor_callback(
            std::shared_ptr<processor>* p, const std::string& include_path,
            std::stack<
                std::shared_ptr<
                    std::pair<
                        std::shared_ptr<std::ifstream>,
                        std::string>>>& input_stack,
            std::function<
                void (const std::pair<directive_type, std::string>&)> prev);
    
    } /* namespace utilities */
} /* namespace minweb */

#endif /*MINWEB_UTILITIES_HEADER_GUARD*/
