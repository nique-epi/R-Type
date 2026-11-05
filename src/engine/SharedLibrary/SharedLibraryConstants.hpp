#pragma once

#include <string_view>

namespace rtype::engine {

/** @brief The suffix of a shared library on Linux and macOS. */
constexpr std::string_view POSIX_LIBRARY_SUFFIX = ".so";

/** @brief The suffix of a shared library on Windows. */
constexpr std::string_view WINDOWS_LIBRARY_SUFFIX = ".dll";

/** @brief What a failure to open a library says when the system gave no
 * reason. */
constexpr std::string_view UNKNOWN_SYSTEM_REASON = "the system gave no reason";

/** @brief What the text of a Windows error ends with after its full stop: a
 * space and a line break, which our own message does not keep. */
constexpr std::string_view SYSTEM_TEXT_TRAILING_CHARACTERS = " \r\n";

}  // namespace rtype::engine
