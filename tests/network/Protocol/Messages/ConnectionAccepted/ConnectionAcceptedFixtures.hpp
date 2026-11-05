#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include "ConnectionAccepted.hpp"

namespace rtype::network::testing {

/** @returns The acceptance of the first player with @p count enemy types,
 * each named @p name. */
[[nodiscard]] ConnectionAccepted acceptanceWithEnemyTypes(
    std::size_t count, std::string_view name);

/**
 * @returns The payload of the acceptance of the first player with one type,
 * written byte by byte as the protocol page lays it out, whatever the side and
 * the name hold: so a test can write what the encoder refuses to write.
 */
[[nodiscard]] std::vector<std::byte> oneTypeAcceptancePayload(
    std::uint8_t side, std::string_view name);

}  // namespace rtype::network::testing
