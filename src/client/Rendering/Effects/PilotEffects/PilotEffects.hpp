#pragma once

#include "EffectTools.hpp"
#include "Entity.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Plays what happens to the ship of a pilot without a shot: its
 * arrival and its invulnerability, and counts the time since the start of
 * the game the arrivals wait on. Every method but advance() and sinceStart()
 * adds components, so none may be called during a forEach of the registry.
 */
class PilotEffects {
 public:
  explicit PilotEffects(EffectTools tools);

  /** @brief Moves the time since the start of the game on. */
  void advance(engine::Duration elapsed);

  /** @returns The time advance() moved on since the effects were built: the
   * time since the start of the game. */
  [[nodiscard]] engine::Duration sinceStart() const;

  /**
   * @brief Plays the arrival of a ship: hidden, then shown row by row over
   * MATERIALIZE_DURATION (see Reveal), behind four speed lines in the light
   * shade of its pilot. Asked before PILOT_ARRIVAL_DELAY into the game, it
   * waits until then. A ship without a Position is left alone.
   */
  void playArrival(engine::Entity ship);

  /** @brief Shows that a ship cannot be hit, or no longer can't: see
   * Invulnerability. Saying it twice changes nothing. */
  void setInvulnerable(engine::Entity ship, bool invulnerable) const;

 private:
  EffectTools tools_;
  engine::Duration sinceStart_{};
};

}  // namespace rtype::client
