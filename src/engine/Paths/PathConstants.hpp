#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::engine {

/**
 * @brief Folder of the files a program reads at run time, found next to its
 * executable or in the folder above it.
 */
constexpr std::string_view ASSETS_FOLDER_NAME = "assets";

/** @brief Link Linux keeps to the executable of the running process. */
constexpr const char* LINUX_EXECUTABLE_LINK = "/proc/self/exe";

/** @brief Longest path Windows returns for a module, in characters. */
constexpr std::size_t MAXIMUM_WINDOWS_PATH_LENGTH = 32768;

}  // namespace rtype::engine
