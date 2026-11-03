#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief Ticks of the server between two sends of a guaranteed message not
 * acknowledged: 100 ms at 60 ticks per second is 6 ticks of 16 666 666 ns, 99
 * 999 996 ns, still short of it; the 7th tick reaches 116 666 662 ns.
 */
constexpr std::size_t TICKS_BEFORE_A_RESEND = 7;

/**
 * @brief The ticks after its first send within which a guaranteed message is
 * dropped: at 60 ticks it is 999 999 960 ns old, under 1 s; at 61, 1 016 666
 * 626 ns. It goes out 9 times before: ticks 0, 7, 14, 21, 28, 35, 42, 49, 56.
 */
constexpr std::size_t TICKS_OF_A_LIFETIME = 61;
constexpr std::size_t SENDS_IN_A_LIFETIME = 9;

/** @brief The flags of a header that neither acknowledges nor holds
 * guaranteed messages. */
constexpr std::uint8_t NO_FLAG = 0;

}  // namespace rtype::server::testing
