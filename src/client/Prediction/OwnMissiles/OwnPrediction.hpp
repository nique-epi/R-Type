#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include "Bounds.hpp"
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
 * commands. The game screen holds one, advances it with each update and
 * gives it each world state.
 */
class OwnPrediction {
 public:
  OwnPrediction(std::uint8_t playerNumber, OwnMissilesParts parts);

  /** @brief Sends and predicts the commands of @p elapsed (see
   * OwnShipPredictor::advance()), the ship stopped against the blocking walls
   * of the registry of its parts, moved ahead by the scroll, and flies the
   * missiles along them. The
   * commands fire only while a state lists the ship and the game is not over:
   * without a ship, the server fires nothing, and the missiles already fired
   * fly on as the server's do. */
  void advance(engine::Duration elapsed, PlayerCommand command,
               IPlayerInputLink& link);

  /** @brief Reads what @p state says of the ship of the player: starts or
   * reconciles the ship, arms the gun again when the ship is a new one, and
   * gives up the shots the server left unanswered. */
  void apply(const network::WorldState& state);

  /** @brief Takes the speed the level scrolls at now, from its LevelStatus:
   * the blocking walls drawn are moved ahead by it (scrollLeadOf()) before the
   * ship is predicted against them. */
  void followScroll(float scrollSpeed);

  /** @brief The game ended: no command fires a missile any more, as the
   * server fires nothing once a game is over; the missiles already fired fly
   * on. */
  void ceaseFire();

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
  std::reference_wrapper<engine::ComponentRegistry> components_;
  float scrollSpeed_{0.0F};
  /** @brief The blocking walls of the update in progress, kept so their room
   * is reused from one update to the next. */
  std::vector<engine::Bounds> obstacles_;
};

}  // namespace rtype::client
