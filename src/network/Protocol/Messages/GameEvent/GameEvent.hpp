#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief What a GameEvent tells. EntityDamaged names an entity and no player;
 * PlayerHit and PlayerEliminated name a player and no entity. The values are
 * the ones of the protocol page.
 */
enum class GameEventKind : std::uint8_t {
  EntityDamaged = 1,
  PlayerHit = 2,
  PlayerEliminated = 3,
};

/**
 * @brief Something that drives a sound or an effect on the client, never a
 * game rule. eventNumber starts at 1 for each game and wraps from 65535 to 0;
 * the three copies of an event carry the same number.
 */
struct GameEvent {
  static constexpr MessageType TYPE = MessageType::GameEvent;

  std::uint16_t eventNumber;
  GameEventKind kind;
  std::uint8_t playerNumber;
  std::uint32_t entityId;

  bool operator==(const GameEvent&) const = default;
};

/**
 * @brief Writes the event number, the kind, the player number, then the
 * entity id.
 * @throws InvalidFieldValueException when the kind is not one of
 * GameEventKind, or when the player number or the entity id does not match
 * what the kind names; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const GameEvent& message);

/**
 * @brief Reads a GameEvent from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when the kind is not one of
 * GameEventKind, or when the player number or the entity id does not match
 * what the kind names.
 * @throws TrailingBytesException when bytes follow the entity id.
 */
[[nodiscard]] GameEvent decodePayload(std::span<const std::byte> payload,
                                      std::type_identity<GameEvent> type);

}  // namespace rtype::network
