/*
 *  Seek debug macros
 *  Author: Maarten Vandersteegen
 */

#ifndef SEEK_DEBUG_H
#define SEEK_DEBUG_H

#include <cstdarg>
#include <cstdio>

namespace LibSeek {
namespace detail {

inline void seek_log(std::FILE* stream, const char* file, int line,
                     const char* func, const char* format, ...)
{
    if (file != nullptr) {
        std::fprintf(stream, "%s:%d:%s(): ", file, line, func);
    }

    va_list args;
    va_start(args, format);
    std::vfprintf(stream, format, args);
    va_end(args);
}

} /* namespace detail */
} /* namespace LibSeek */

#ifdef SEEK_DEBUG
#define debug(...) LibSeek::detail::seek_log(stdout, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define error(...) LibSeek::detail::seek_log(stderr, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define debug(...) ((void)0)
#define error(...) LibSeek::detail::seek_log(stderr, nullptr, 0, nullptr, __VA_ARGS__)
#endif

#endif /* SEEK_DEBUG_H */
