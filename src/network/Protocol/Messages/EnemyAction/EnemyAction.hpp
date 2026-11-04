#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Where an attack stands: Signal is the warning before it, Strike the
 * attack itself, Recover the moment after, Enraged the change of a boss that
 * went under half of its health. The values are the ones of the protocol
 * page.
 */
enum class EnemyActionPhase : std::uint8_t {
  Signal = 1,
  Strike = 2,
  Recover = 3,
  Enraged = 4,
};

/**
 * @brief An enemy enters a phase of an attack. Guaranteed: sent again until
 * acknowledged, for GUARANTEED_MESSAGE_LIFETIME at most.
 *
 * `attack` is a number each kind of enemy gives its own attacks; the protocol
 * does not list them. The client ignores a pair (attack, phase) its table of
 * the kind has no animation for. A second copy (same three fields) arriving
 * within two seconds of the first changes nothing.
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
