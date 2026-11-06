#pragma once

#include <functional>
#include <span>
#include <string>

namespace rtype::engine {

/**
 * @brief Tells, from the words of a line of the configuration, whether the
 * line belongs to another program: the reader does not run such a line, it
 * keeps it as it was written for that program. Empty: every line belongs to
 * the program that reads it.
 */
using HandOverRule = std::function<bool(std::span<const std::string> words)>;

}  // namespace rtype::engine
