#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "Endpoint.hpp"
#include "EntityIdConstants.hpp"
#include "PlayerInput.hpp"

namespace rtype::server {

/**
 * @brief A player connected to the game: who the player is to the others, and
 * the session the server keeps with them.
 */
struct ConnectedPlayer {
  /** @brief FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS; it also picks the color
   * of the player. */
  std::uint8_t playerNumber{};
  /** @brief 1 to MAXIMUM_NICKNAME_SIZE bytes of UTF-8, as the request carried
   * it. */
  std::string nickname;
  /** @brief The address and port the player sends from, where every datagram
   * to the player goes. */
  network::Endpoint endpoint;
  /** @brief Drawn at random when the player was accepted, never 0; every
   * datagram of the session carries it. */
  std::uint64_t sessionToken{};
  /** @brief Sequence of the next datagram sent to the player: 0 for the first
   * one, then one more each time, wrapping from 65535 to 0. */
  std::uint16_t nextSequence{};
  /** @brief The network id of the ship of the player, NO_ENTITY_ID while it
   * has none. */
  std::uint32_t shipEntityId{identifiers::NO_ENTITY_ID};
  /** @brief Whether the player declared itself ready to start; cleared when the
   * game waits for players again. */
  bool ready{false};
  /** @brief The sequence of the newest datagram accepted from the player for
   * the messages that need an order, nothing before the first. */
  std::optional<std::uint16_t> newestReceivedSequence;
  /** @brief The PlayerInput with the highest inputNumber the player sent
   * during the session, to apply at every tick until a newer one arrives;
   * empty until the first one arrives. PlayerInputReceiver keeps it. */
  std::optional<network::PlayerInput> newestInput{std::nullopt};
};

}  // namespace rtype::server
