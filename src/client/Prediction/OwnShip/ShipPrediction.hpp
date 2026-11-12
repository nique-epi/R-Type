#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <span>
#include <vector>
#include "Bounds.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipDirections.hpp"

namespace rtype::client {

/** @brief A command sent and not yet confirmed: its number, the directions it
 * held, and the ship as the client predicted it right after it. */
struct PendingCommand {
  std::uint32_t inputNumber{};
  game::ShipDirections held{};
  game::MovingShip after{};
};

/**
 * @brief Where the player's own ship is, as the client predicts it: the same
 * shipAfter() as the server, one simulation tick per command, stopped against
 * the blocking obstacles it was shown (seeObstacles()), corrected from the
 * position the server confirms.
 *
 * Each command is numbered and kept until the server confirms a newer one.
 * When the server says it received command N and puts the ship at P, the
 * commands before N are dropped; if the ship predicted after N is farther than
 * RECONCILIATION_TOLERANCE from P, the prediction restarts from P with the
 * motion kept after N and replays the commands after N. A server within the
 * tolerance changes nothing, so the ship never jumps for a small difference.
 *
 * The prediction knows nothing of the network: it is given the numbers of the
 * commands and the confirmations.
 */
class ShipPrediction {
 public:
  /** @brief Starts predicting from @p start, a ship at rest at the position
   * the server gave it once it applied the command @p acknowledged, then plays
   * the commands kept after it again: the server applies them to that ship
   * too, so the prediction stays as far ahead of the server as before. */
  void restart(game::Position start, std::uint32_t acknowledged);

  /** @brief Moves the ship one tick holding @p held, as the command
   * @p inputNumber, and keeps the command. Before restart(), only keeps it,
   * at most KEPT_COMMANDS_BEFORE_A_SHIP of them, for restart() to play. */
  void predict(game::ShipDirections held, std::uint32_t inputNumber);

  /** @brief Takes into account the server putting the ship at @p server after
   * the newest command it applied, @p acknowledged. Nothing for 0, for a
   * number no pending command carries, or before restart(). */
  void reconcile(game::Position server, std::uint32_t acknowledged);

  /** @brief Stops the ship against @p obstacles from the next command on,
   * the commands played again included, as the server stops it
   * (blockedByObstacles()). */
  void seeObstacles(std::span<const engine::Bounds> obstacles);

  [[nodiscard]] bool isStarted() const;
  [[nodiscard]] game::Position position() const;

 private:
  void dropBefore(std::uint32_t inputNumber);
  void replayAfterFront(game::MovingShip restart);

  /** @brief Plays the pending commands from the one at @p first again, from
   * @p from, each one tick, and keeps where each leaves the ship. The first
   * one follows the command before it, or itself when there is none: a key
   * held through a respawn presses nothing, as on the server. */
  void replayFrom(std::size_t first, game::MovingShip from);

  bool started_{false};
  game::MovingShip current_{};
  game::ShipDirections lastHeld_{};
  std::deque<PendingCommand> pending_;
  std::vector<engine::Bounds> obstacles_;
};

}  // namespace rtype::client
