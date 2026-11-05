#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The step of an attack an enemy enters. The values are the ones of the
 * protocol page.
 */
enum class EnemyActionPhase : std::uint8_t {
  Signal = 1,
  Strike = 2,
  Recover = 3,
  Enraged = 4,
};

/**
 * @brief An enemy enters a step of one of its attacks. Guaranteed: sent again
 * until acknowledged, for GUARANTEED_MESSAGE_LIFETIME at most. The receiver
 * keeps the first copy it sees of an (entity, attack, phase) and drops the
 * others.
 */
struct EnemyAction {
  static constexpr MessageType TYPE = MessageType::EnemyAction;

  std::uint32_t entityId;
  std::uint8_t attack;
  EnemyActionPhase phase;

  bool operator==(const EnemyAction&) const = default;
};

/**
 * @brief Writes the entity id, the attack, then the phase.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * phase is not one of EnemyActionPhase; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const EnemyAction& message);

/**
 * @brief Reads an EnemyAction from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * phase is not one of EnemyActionPhase.
 * @throws TrailingBytesException when bytes follow the phase.
 */
[[nodiscard]] EnemyAction decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<EnemyAction> type);

}  // namespace rtype::network
