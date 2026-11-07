#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "Bounds.hpp"
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "IOwnMissileListener.hpp"
#include "OwnMissile.hpp"
#include "OwnMissilesParts.hpp"
#include "Position.hpp"
#include "PredictedCommand.hpp"
#include "PredictedGun.hpp"

namespace rtype::client {

/**
 * @brief The missiles of the player of this client, fired by the client
 * itself from its predicted ship, as Unity Netcode predicts the spawn of a
 * projectile and Unreal Tournament fires a fake one.
 *
 * Each command the prediction sends counts down the PredictedGun; a command
 * that fires creates a missile at the muzzle of the ship as predicted right
 * after it, with its shot and its muzzle flash, and the missile flies at the
 * velocity of the gun, on the timeline of the predicted ship: the missile of
 * input F is drawn, at the newest command N, (N - F) ticks of flight from its
 * start, where the server has it at the tick it applies N.
 *
 * The replicator hands it the EntitySpawned of the missiles of its player
 * (see IOwnMissileListener) and draws none of them. A missile of the server
 * is matched to the missile of the same firing input: the missile keeps the
 * start of the server, and the gun is put back where the server had it, so a
 * shot predicted wrongly is taken away and a shot the client did not predict
 * appears where it flies. A shot the server leaves unanswered for
 * REFUSED_SHOT_TICKS after it applied its input is taken away.
 *
 * A missile that touches an enemy drawn on screen plays its impact and is
 * gone for good, whatever the server decides: the server tests it where the
 * player saw the enemies, so it counts the same hit. When the server removes a
 * missile still drawn, it goes without an impact: the enemy shows the hit. A
 * missile is never drawn again once gone.
 */
class OwnMissiles final : public IOwnMissileListener {
 public:
  explicit OwnMissiles(OwnMissilesParts parts);

  /** @brief The ship of the player appeared: arms the gun of the ships of
   * the session, its first shot firstShotTicks away. */
  void restart();

  /** @brief Fires the commands of @p commands that fire, oldest first, then
   * draws every missile at the newest of them. */
  void follow(std::span<const PredictedCommand> commands);

  /** @brief The server applied the inputs up to @p acknowledged: the shots
   * still unanswered REFUSED_SHOT_TICKS before it are taken away. */
  void acknowledge(std::uint32_t acknowledged);

  /** @brief Plays the impact of every missile that touches an enemy drawn on
   * screen, and takes that missile away for good. Not during a forEach. */
  void touchEnemies();

  /** @brief Turns the prediction of the missiles on or off; off takes every
   * missile away. */
  void setActive(bool active);

  [[nodiscard]] bool predictsOwnMissiles() const override;

  void ownMissileSpawned(const network::EntitySpawned& missile) override;

  void ownMissileDestroyed(const network::EntityDestroyed& destroyed) override;

 private:
  /** @returns The missile of @p firingInput, created at @p start with its
   * shot and its muzzle flash; nullptr, and nothing created, when the session
   * gives no look for it. */
  OwnMissile* fire(std::uint32_t firingInput, game::Position start);

  /** @brief Puts every missile drawn where it flies at the newest command. */
  void place();

  OwnMissilesParts parts_;
  PredictedGun gun_;
  std::vector<OwnMissile> missiles_;
  std::vector<engine::Bounds> enemyBoxes_;
  std::uint32_t newestInput_{0};
  bool active_{false};
};

}  // namespace rtype::client
