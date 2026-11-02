#pragma once

#include <cstddef>
#include <functional>
#include <span>
#include <string_view>

namespace rtype::engine {

/**
 * @brief Reads the options of a library that reads its own: the logging and
 * the network simulator, which may not link the engine and so cannot describe
 * their options as LaunchOption.
 *
 * readLaunchArguments() hands it the arguments from the one it has reached to
 * the last, only at an argument that is neither an option of the program nor
 * the value of one. It reads the first of them when it is one of its options,
 * and returns how many arguments that option took: 1 for an option alone, 2
 * for an option and its value, which is the next argument even when it looks
 * like an option. It returns 0, and reads nothing, when the first argument is
 * none of its options. It may throw to refuse an option or its value.
 */
using LaunchOptionReader =
    std::function<std::size_t(std::span<const std::string_view> arguments)>;

}  // namespace rtype::engine
