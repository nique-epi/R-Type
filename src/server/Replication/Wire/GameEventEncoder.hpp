#pragma once

#include <cstdint>
#include "BossArrival.hpp"
#include "BossHealthChanged.hpp"
#include "EnemyActionStarted.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EventMessage.hpp"
#include "GameEvent.hpp"
#include "GameEventConstants.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"

namespace rtype::server {

/**
 * @brief Writes the events of the match as the messages of the protocol, says
 * which of them are guaranteed, and numbers the GameEvents of a game.
 *
 * Every encode() throws what the encodePayload() of its message throws,
 * InvalidFieldValueException for an entity id of 0, and changes nothing then:
 * an event number is only used once its message is written.
 */
class GameEventEncoder {
 public:
  /** @returns An EntitySpawned, sent once: the next WorldState creates the
   * entity when it is lost. */
  [[nodiscard]] static EventMessage encode(const game::EntitySpawn& event);

  /** @returns An EntityDestroyed, guaranteed. */
  [[nodiscard]] static EventMessage encode(
      const game::EntityDestruction& event);

  /** @returns An EnemyAction, guaranteed. */
  [[nodiscard]] static EventMessage encode(
      const game::EnemyActionStarted& event);

  /** @returns A GameEvent of kind EntityDamaged with the next event number,
   * guaranteed. */
  [[nodiscard]] EventMessage encode(const game::EntityDamage& event);

  /** @returns A GameEvent of kind PlayerHit with the next event number,
   * guaranteed. */
  [[nodiscard]] EventMessage encode(const game::PlayerHit& event);

  /** @returns A GameEvent of kind PlayerEliminated with the next event number,
   * guaranteed. */
  [[nodiscard]] EventMessage encode(const game::PlayerElimination& event);

  /** @returns A PlayerLeft with the number of the player and why it left,
   * guaranteed. */
  [[nodiscard]] static EventMessage encode(const game::PlayerDeparture& event);
  /** @returns A BossStatus of a boss arriving, guaranteed. */
  [[nodiscard]] static EventMessage encode(const game::BossArrival& event);
  /** @returns A BossStatus of the boss in play, guaranteed: Defeated with no
   * point left, Enraged once enraged, Fighting otherwise. */
  [[nodiscard]] static EventMessage encode(
      const game::BossHealthChanged& event);

 private:
  [[nodiscard]] EventMessage encodeGameEvent(network::GameEventKind kind,
                                             std::uint8_t playerNumber,
                                             std::uint32_t entityId);

  std::uint16_t nextEventNumber_{FIRST_EVENT_NUMBER};
};

}  // namespace rtype::server
