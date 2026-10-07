#pragma once

#include <cstddef>

namespace rtype::client {

constexpr const char* LINUX_EXECUTABLE_LINK = "/proc/self/exe";

/** @brief Longest path Windows returns for a module, in characters. */
constexpr std::size_t MAXIMUM_WINDOWS_PATH_LENGTH = 32768;

}  // namespace rtype::client
