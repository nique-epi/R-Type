#pragma once

#include <cstdint>

namespace rtype::network::testing {

/** @brief The port of a game process, 0x1234, and the port no player can
 * reach. */
constexpr std::uint16_t REDIRECT_PORT = 0x1234;
constexpr std::uint16_t UNREACHABLE_PORT = 0;

}  // namespace rtype::network::testing
