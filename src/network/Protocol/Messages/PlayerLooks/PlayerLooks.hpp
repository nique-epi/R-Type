#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "ShipLook.hpp"

namespace rtype::network {

/** @brief The look of one player of a game. */
struct PlayerLook {
  std::uint8_t playerNumber{};
  identifiers::ShipLook look;

  bool operator==(const PlayerLook&) const = default;
};

/**
 * @brief The look of every player of the game, in the order of their numbers.
 * The server sends it guaranteed to every player when one changes its look and
 * when a player arrives, and to one player when it answers a look unchanged.
 */
struct PlayerLooks {
  static constexpr MessageType TYPE = MessageType::PlayerLooks;

  std::vector<PlayerLook> players;

  bool operator==(const PlayerLooks&) const = default;
};

/**
 * @returns The look @p looks lists for the player numbered @p number, or
 * nullptr when it does not list that player. The pointer stays valid as long
 * as @p looks does.
 */
[[nodiscard]] const identifiers::ShipLook* findLook(
    std::span<const PlayerLook> looks, std::uint8_t number);

/**
 * @brief Writes the number of players, then a player number and a look for
 * each.
 * @throws TooManyPlayersException when more than MAXIMUM_PLAYERS are listed.
 * @throws InvalidFieldValueException for a player number or a look the
 * protocol does not allow.
 * @throws DuplicatePlayerNumberException when a number appears twice.
 */
void encodePayload(ByteWriter& writer, const PlayerLooks& message);

/**
 * @brief Reads PlayerLooks from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws TooManyPlayersException, InvalidFieldValueException or
 * DuplicatePlayerNumberException for a value the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the last player.
 */
[[nodiscard]] PlayerLooks decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<PlayerLooks> type);

}  // namespace rtype::network
