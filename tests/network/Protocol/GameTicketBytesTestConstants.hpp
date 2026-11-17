#pragma once

#include <cstddef>
#include <string_view>
#include "GameTicket.hpp"

namespace rtype::network::testing {

/** @brief The game ticket of a player signed in to the master, its bytes
 * 0x20 to 0x3F, each different, so a byte written out of order shows. */
constexpr GameTicket SOME_TICKET{
    .bytes = {
        std::byte{0x20}, std::byte{0x21}, std::byte{0x22}, std::byte{0x23},
        std::byte{0x24}, std::byte{0x25}, std::byte{0x26}, std::byte{0x27},
        std::byte{0x28}, std::byte{0x29}, std::byte{0x2A}, std::byte{0x2B},
        std::byte{0x2C}, std::byte{0x2D}, std::byte{0x2E}, std::byte{0x2F},
        std::byte{0x30}, std::byte{0x31}, std::byte{0x32}, std::byte{0x33},
        std::byte{0x34}, std::byte{0x35}, std::byte{0x36}, std::byte{0x37},
        std::byte{0x38}, std::byte{0x39}, std::byte{0x3A}, std::byte{0x3B},
        std::byte{0x3C}, std::byte{0x3D}, std::byte{0x3E}, std::byte{0x3F}}};

/** @brief That ticket as the master writes it. */
constexpr std::string_view SOME_TICKET_TEXT =
    "rtt_202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f";

/** @brief The ticket the protocol page gives a guest: 32 bytes of 0. */
constexpr GameTicket GUEST_TICKET_ON_THE_PAGE{};

}  // namespace rtype::network::testing
