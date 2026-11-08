#pragma once

#include <functional>
#include <string>

namespace rtype::game {

/** @brief What a scripted behaviour calls the first time its script fails:
 * the program writes the message where its host reads it. */
using ScriptFailureReport = std::function<void(const std::string& message)>;

}  // namespace rtype::game
