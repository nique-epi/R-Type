#pragma once

#include <cstdint>
#include "EntityDamage.hpp"
#include "EventMessage.hpp"
#include "GameEvent.hpp"
#include "GameEventConstants.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"

namespace rtype::server {

/**
 * @brief Writes the events of the match that become a GameEvent, which it
 * numbers, and says they are guaranteed. The other events need no number:
 * eventMessageOf() writes them (EventMessages.hpp).
 *
 * Every encode() throws what the encodePayload() of GameEvent throws,
 * InvalidFieldValueException for an entity id of 0, and changes nothing then:
 * an event number is only used once its message is written.
 */
class GameEventEncoder {
 public:
  /** @returns A GameEvent of kind EntityDamaged with the next event number,
   * guaranteed. */
  [[nodiscard]] EventMessage encode(const game::EntityDamage& event);

  /** @returns A GameEvent of kind PlayerHit with the next event number,
   * guaranteed. */
  [[nodiscard]] EventMessage encode(const game::PlayerHit& event);

  /** @returns A GameEvent of kind PlayerEliminated with the next event number,
   * guaranteed. */
  [[nodiscard]] EventMessage encode(const game::PlayerElimination& event);

 private:
  [[nodiscard]] EventMessage encodeGameEvent(network::GameEventKind kind,
                                             std::uint8_t playerNumber,
                                             std::uint32_t entityId);

  std::uint16_t nextEventNumber_{FIRST_EVENT_NUMBER};
};

}  // namespace rtype::server
