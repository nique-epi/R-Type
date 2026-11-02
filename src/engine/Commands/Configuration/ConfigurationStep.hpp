#pragma once

#include <functional>
#include "CommandInterpreter.hpp"

namespace rtype::engine {

/**
 * @brief What a program runs between the install and the start of its
 * modules: it reads the configuration and runs it as commands of the
 * interpreter, which then holds the commands every module registered.
 */
using ConfigurationStep = std::function<void(CommandInterpreter&)>;

}  // namespace rtype::engine
