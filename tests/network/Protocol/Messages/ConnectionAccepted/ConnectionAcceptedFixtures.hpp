#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ShipGun.hpp"

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

/**
 * @returns The payload of the acceptance of player @p playerNumber with one
 * enemy type named a, written byte by byte as the protocol page lays it out:
 * its box of 0 by 0 and a gun of zeros, which predicts nothing.
 */
[[nodiscard]] std::vector<std::byte> acceptancePayloadOfPlayer(
    std::uint8_t playerNumber);

/**
 * @returns An acceptance of the first player with a ship, SHIP_TYPE_NUMBER,
 * and its missile, MISSILE_TYPE_NUMBER, whose gun is @p gun.
 */
[[nodiscard]] ConnectionAccepted acceptanceWithGun(ShipGun gun);

}  // namespace rtype::network::testing
