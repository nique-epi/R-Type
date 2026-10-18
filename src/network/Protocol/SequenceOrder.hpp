#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Half of the 16-bit sequence range; a distance of exactly this is
 * never newer.
 */
constexpr std::uint16_t SEQUENCE_HALF_RANGE = 32768;

/**
 * @brief Tells whether a datagram is newer than the newest one accepted so
 * far, with the serial number arithmetic of RFC 1982.
 *
 * @returns Whether @p candidate is ahead of @p newest by 1 to 32767, counting
 * across the wrap from 65535 to 0. The same sequence (a duplicate) and a
 * distance of exactly 32768 are not newer.
 */
[[nodiscard]] bool isNewerSequence(std::uint16_t candidate,
                                   std::uint16_t newest);

}  // namespace rtype::network
