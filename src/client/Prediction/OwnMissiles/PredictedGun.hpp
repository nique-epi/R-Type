#pragma once

#include <cstdint>
#include <deque>
#include <vector>
#include "Gun.hpp"
#include "PredictedCommand.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/**
 * @brief The gun of the own ship as the client predicts it: the count down of
 * WeaponSystem (countsDownToReady()), run on the commands the client sends, one
 * tick each. Each command counts one tick off the gun, and fires when the gun
 * is then ready and the command holds shoot, which starts the interval again.
 * The commands are kept until the server confirms them, so a shot the server
 * confirms puts the gun back where the server had it and plays the commands
 * after it again.
 */
class PredictedGun {
 public:
  /**
   * @brief Takes @p gun as the gun of the ships, as the server arms the gun
   * of a new ship: its first shot firstShotTicks away from the input after
   * @p acknowledged, the newest one the server applied when its state listed
   * the ship. Drops the commands up to it, then plays the ones kept after it.
   * @returns The commands after @p acknowledged that fire then, oldest first:
   * the client sent them before it knew the ship, the server fires them.
   */
  [[nodiscard]] std::vector<PredictedCommand> arm(network::ShipGun gun,
                                                  std::uint32_t acknowledged);

  /** @returns Whether @p command fires, and keeps it. */
  [[nodiscard]] bool fires(const PredictedCommand& command);

  /** @brief Keeps @p command without counting it, while the player has no
   * ship to fire from: arm() plays it once a ship appears. */
  void record(const PredictedCommand& command);

  /**
   * @brief Puts the gun where the server had it once it fired for the input
   * @p firingInput, then plays the kept commands after it again.
   * @returns The commands after @p firingInput that fire then, oldest first.
   */
  [[nodiscard]] std::vector<PredictedCommand> confirm(
      std::uint32_t firingInput);

  /** @brief Drops the commands numbered @p acknowledged or lower. */
  void forgetUpTo(std::uint32_t acknowledged);

 private:
  /** @returns Whether one more tick with @p shooting fires. */
  bool countsDown(bool shooting);

  game::Gun countDown_{};
  std::deque<PredictedCommand> commands_;
};

}  // namespace rtype::client
