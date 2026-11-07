#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectionAccepted.hpp"

namespace rtype::network::testing {

/** @returns The acceptance of the first player, announcing a pack whose
 * fingerprint holds the bytes 0 to 31, of @p size bytes, on SOME_PACK_PORT. */
[[nodiscard]] ConnectionAccepted acceptanceOfAPackOf(std::uint32_t size);

/** @returns The payload of acceptanceOfAPackOf(SOME_PACK_SIZE), written byte
 * by byte as the protocol page lays it out. */
[[nodiscard]] std::vector<std::byte> payloadOnTheProtocolPage();

}  // namespace rtype::network::testing
