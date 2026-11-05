#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief One player of a game, as a WorldState lists it.
 */
struct PlayerState {
  std::uint8_t playerNumber{};
  std::uint32_t shipEntityId{};
  std::uint8_t lives{};
  std::uint32_t score{};
  bool invulnerable{false};
  /** @brief What the pilot does: the directions of its newest input, at
   * their bits of the actions (DIRECTION_ACTION_BITS), and the maneuver of its
   * ship (ROLLING_MOTION_BIT_POSITION, DASHING_MOTION_BIT_POSITION). */
  std::uint8_t motion{};
  /** @brief The inputNumber of the newest PlayerInput the server applied to
   * the ship of this player, 0 while it applied none. */
  std::uint32_t acknowledgedInput{};

  bool operator==(const PlayerState&) const = default;
};

/**
 * @brief One entity of a game: its network id, the number of its type in the
 * table of the ConnectionAccepted of the session, and where its center is, in
 * logical units.
 */
struct EntityState {
  std::uint32_t entityId{};
  std::uint8_t entityType{};
  float x{};
  float y{};

  bool operator==(const EntityState&) const = default;
};

/**
 * @brief The state of the game at one server tick.
 *
 * `tick` counts the simulation ticks of the server since the game started. A
 * state too big for one datagram is sent as several, with the same tick; each
 * holds every player and a part of the entities.
 */
struct WorldState {
  static constexpr MessageType TYPE = MessageType::WorldState;

  std::uint32_t tick;
  std::vector<PlayerState> players;
  std::vector<EntityState> entities;

  bool operator==(const WorldState&) const = default;
};

/**
 * @brief Reads one WorldState payload, as WorldStateCodec::decode() does. A
 * state too big for one datagram is written by WorldStateCodec::encode(),
 * which gives several payloads, so there is no encodePayload() for it.
 * @throws What WorldStateCodec::decode() throws.
 */
[[nodiscard]] WorldState decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<WorldState> type);

}  // namespace rtype::network
