#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>
#include "PredictedCommand.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/**
 * @brief The gun of the own ship as the client predicts it: the count down of
 * WeaponSystem, run on the commands the client sends, one tick each. Each
 * command counts one tick off the gun, and fires when the gun is then ready
 * and the command holds shoot, which starts the interval again. The commands
 * are kept until the server confirms them, so a shot the server confirms puts
 * the gun back where the server had it and plays the commands after it again.
 */
class PredictedGun {
 public:
  /** @brief Takes @p gun as the gun of the ships and restarts. */
  void arm(network::ShipGun gun);

  /** @brief The ship appeared: the first shot is firstShotTicks away, and no
   * command is kept. */
  void restart();

  /** @returns Whether @p command fires, and keeps it. */
  [[nodiscard]] bool fires(const PredictedCommand& command);

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

  network::ShipGun gun_{};
  std::size_t ticksUntilReady_{0};
  std::deque<PredictedCommand> commands_;
};

}  // namespace rtype::client
