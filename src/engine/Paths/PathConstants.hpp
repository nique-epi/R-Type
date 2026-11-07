#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::engine {

/**
 * @brief Folder of the files a program reads at run time, found next to its
 * executable, in the folder above it, or in the Resources folder of its app.
 */
constexpr std::string_view ASSETS_FOLDER_NAME = "assets";

/**
 * @brief Folder of a macOS app that holds its resources, next to the folder of
 * its executable.
 */
constexpr std::string_view APP_RESOURCES_FOLDER_NAME = "Resources";

/** @brief Link Linux keeps to the executable of the running process. */
constexpr const char* LINUX_EXECUTABLE_LINK = "/proc/self/exe";

/** @brief Longest path Windows returns for a module, in characters. */
constexpr std::size_t MAXIMUM_WINDOWS_PATH_LENGTH = 32768;

}  // namespace rtype::engine
