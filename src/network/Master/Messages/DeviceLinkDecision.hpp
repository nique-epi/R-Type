#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief What a signed-in player decided on the web site about a device
 * link: connect the game to its account, or refuse. */
enum class DeviceLinkDecision : std::uint8_t { Approved, Denied };

}  // namespace rtype::network
