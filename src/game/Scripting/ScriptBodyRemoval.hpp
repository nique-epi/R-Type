#pragma once

#include <cstdint>
#include <functional>

namespace rtype::game {

/** @brief What a scripted behaviour calls to remove from the game the body of
 * a network id whose script failed. */
using ScriptBodyRemoval = std::function<void(std::uint32_t networkId)>;

}  // namespace rtype::game
