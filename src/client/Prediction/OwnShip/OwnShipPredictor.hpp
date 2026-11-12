#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "Bounds.hpp"
#include "IPlayerInputLink.hpp"
#include "PlayerCommand.hpp"
#include "Position.hpp"
#include "PredictedCommand.hpp"
#include "ShipPrediction.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/**
 * @brief Moves the own ship of the player before the server answers: it sends
 * one command per simulation tick, predicts it with ShipPrediction, and takes
 * the confirmation of the server from each WorldState.
 *
 * The server moves a ship in whole ticks of SIMULATION_TICK_DURATION, and its
 * drift depends on the length of the step, so the prediction counts the real
 * time into whole ticks the same way, at most MAXIMUM_TICKS_PER_ADVANCE per
 * call: the time past them is dropped, as a stalled server drops it. The
 * prediction starts from the first state that lists the ship, and starts
 * again from a ship with another entity (the player came back after a hit).
 */
class OwnShipPredictor {
 public:
  /** @param playerNumber The number the server gave the player. */
  explicit OwnShipPredictor(std::uint8_t playerNumber);

  /**
   * @brief Counts @p elapsed into ticks, and for each sends @p link the
   * actions and the shown tick of @p command, then predicts the number it was
   * sent under; nothing is predicted for a command the link did not send.
   * @returns The commands predicted, oldest first, with the ship after each.
   */
  std::vector<PredictedCommand> advance(engine::Duration elapsed,
                                        PlayerCommand command,
                                        IPlayerInputLink& link);

  /** @brief Reads what @p state says of the ship of the player: forgets the
   * ship followed when the state lists the player with another ship or none,
   * as once it was destroyed; then starts the prediction, or reconciles it.
   * A part of a state that lists the player with the ship followed but not
   * its entity changes nothing. */
  void apply(const network::WorldState& state);

  /** @brief Stops the predicted ship against @p obstacles from the next
   * command on (see ShipPrediction::seeObstacles()). */
  void seeObstacles(std::span<const engine::Bounds> obstacles);

  /** @returns The entity id the server gave the ship, once a state listed it
   * and the prediction runs. */
  [[nodiscard]] std::optional<std::uint32_t> shipEntityId() const;

  /** @returns Where the prediction puts the ship. */
  [[nodiscard]] game::Position position() const;

 private:
  std::uint8_t playerNumber_;
  ShipPrediction prediction_;
  std::optional<std::uint32_t> shipEntityId_;
  engine::Duration uncounted_{};
};

}  // namespace rtype::client
