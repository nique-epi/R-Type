#pragma once

#include <cstdint>
#include <deque>
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
 * shipAfter() as the server, one simulation tick per command, corrected from
 * the position the server confirms.
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
   * of the server, and forgets every pending command. */
  void restart(game::Position start);

  /** @brief Moves the ship one tick holding @p held, as the command
   * @p inputNumber, and keeps the command. Nothing before restart(). */
  void predict(game::ShipDirections held, std::uint32_t inputNumber);

  /** @brief Takes into account the server putting the ship at @p server after
   * the newest command it applied, @p acknowledged. Nothing for 0, for a
   * number no pending command carries, or before restart(). */
  void reconcile(game::Position server, std::uint32_t acknowledged);

  [[nodiscard]] bool isStarted() const;
  [[nodiscard]] game::Position position() const;

 private:
  void dropBefore(std::uint32_t inputNumber);
  void replayAfterFront(game::MovingShip restart);

  bool started_{false};
  game::MovingShip current_{};
  game::ShipDirections lastHeld_{};
  std::deque<PendingCommand> pending_;
};

}  // namespace rtype::client
