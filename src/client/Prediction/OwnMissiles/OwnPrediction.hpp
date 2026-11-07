#pragma once

#include <cstdint>
#include "ComponentRegistry.hpp"
#include "EntityReplicator.hpp"
#include "IOwnMissileListener.hpp"
#include "IPlayerInputLink.hpp"
#include "OwnMissiles.hpp"
#include "OwnMissilesParts.hpp"
#include "OwnShipPredictor.hpp"
#include "PlayerCommand.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/**
 * @brief What the client predicts of its own player: its ship
 * (OwnShipPredictor) and the missiles it fires (OwnMissiles), from the same
 * commands. The game screen holds one, advances it with each update while
 * PREDICTION is on, stops it while it is off, and gives it each world state.
 */
class OwnPrediction {
 public:
  OwnPrediction(std::uint8_t playerNumber, OwnMissilesParts parts);

  /** @brief Sends and predicts the commands of @p elapsed (see
   * OwnShipPredictor::advance()), and fires the missiles they fire while a
   * state lists the ship and the game is not over: without a ship, the
   * server fires nothing. */
  void advance(engine::Duration elapsed, PlayerCommand command,
               IPlayerInputLink& link);

  /** @brief Reads what @p state says of the ship of the player: starts or
   * reconciles the ship, arms the gun again when the ship is a new one, and
   * gives up the shots the server left unanswered. */
  void apply(const network::WorldState& state);

  /** @brief The game ended: no command fires a missile any more, and the
   * missiles drawn stay where they are, as the server runs nothing once a
   * game is over. */
  void ceaseFire();

  /** @brief Stops predicting: forgets the ship and takes every missile away,
   * as when the player turns PREDICTION off. */
  void stop();

  /** @brief Puts the ship of the player where it is predicted (see
   * placePredictedShip()), then plays the impact of every missile that
   * touches an enemy drawn. Not during a forEach of @p components. */
  void place(const EntityReplicator& replicator,
             engine::ComponentRegistry& components);

  /** @returns Who draws the missiles of the player, for the replicator. */
  [[nodiscard]] IOwnMissileListener& missiles();

 private:
  std::uint8_t playerNumber_;
  OwnShipPredictor ship_;
  OwnMissiles missiles_;
  bool firing_{true};
};

}  // namespace rtype::client
