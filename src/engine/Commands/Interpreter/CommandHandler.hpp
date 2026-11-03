#pragma once

#include <functional>
#include <span>
#include <string>

namespace rtype::engine {

/**
 * @brief What runs a command, given the words after its name. It throws a
 * CommandRefusedException to refuse them.
 */
using CommandHandler = std::function<void(std::span<const std::string>)>;

}  // namespace rtype::engine
