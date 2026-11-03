#pragma once

#include <cstdint>
#include "EncodedMessage.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "GameEvent.hpp"
#include "GameEventConstants.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"

namespace rtype::server {

/**
 * @brief Writes the events of the match as the messages of the protocol, and
 * numbers the GameEvents of a game.
 *
 * Every encode() throws what the encodePayload() of its message throws,
 * InvalidFieldValueException for an entity id of 0, and changes nothing then:
 * an event number is only used once its message is written.
 */
class GameEventEncoder {
 public:
  /**
   * @returns An EntitySpawned, sent SINGLE_COPY time.
   */
  [[nodiscard]] static EncodedMessage encode(const game::EntitySpawn& event);

  /**
   * @returns An EntityDestroyed, sent REPEATED_MESSAGE_COPIES times.
   */
  [[nodiscard]] static EncodedMessage encode(
      const game::EntityDestruction& event);

  /**
   * @returns A GameEvent of kind EntityDamaged with the next event number,
   * sent REPEATED_MESSAGE_COPIES times.
   */
  [[nodiscard]] EncodedMessage encode(const game::EntityDamage& event);

  /**
   * @returns A GameEvent of kind PlayerHit with the next event number, sent
   * REPEATED_MESSAGE_COPIES times.
   */
  [[nodiscard]] EncodedMessage encode(const game::PlayerHit& event);

  /**
   * @returns A GameEvent of kind PlayerEliminated with the next event number,
   * sent REPEATED_MESSAGE_COPIES times.
   */
  [[nodiscard]] EncodedMessage encode(const game::PlayerElimination& event);

  /**
   * @returns A PlayerLeft with the number of the player and why it left, sent
   * REPEATED_MESSAGE_COPIES times.
   */
  [[nodiscard]] static EncodedMessage encode(
      const game::PlayerDeparture& event);

 private:
  [[nodiscard]] EncodedMessage encodeGameEvent(network::GameEventKind kind,
                                               std::uint8_t playerNumber,
                                               std::uint32_t entityId);

  std::uint16_t nextEventNumber_{FIRST_EVENT_NUMBER};
};

}  // namespace rtype::server
