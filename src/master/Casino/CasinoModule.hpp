#pragma once

#include <optional>
#include <string>
#include <vector>
#include "CasinoController.hpp"
#include "CasinoRoundJob.hpp"
#include "CasinoService.hpp"
#include "CasinoSettings.hpp"
#include "ConfigurationRegistry.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "RouletteDealer.hpp"
#include "SettingValue.hpp"
#include "SystemRouletteDraw.hpp"

namespace rtype::master {

/**
 * @brief The module of the casino of the master: roulette rooms whose rounds
 * follow the clock, the same in every room, kept in the tables
 * `casino_seats`, `casino_bets` and `casino_rounds`. It answers
 * `GET /api/casino/rooms` and `GET /api/casino/rooms/{room}` to everyone, and
 * `PUT` and `DELETE /api/casino/rooms/{room}/seats/me`, `PUT` and
 * `GET /api/casino/rooms/{room}/rounds/{round}/bets/me` over HTTPS only, to
 * the account of a session; the chips move through the IAccountWallet of the
 * module wallet.
 *
 * It needs the module wallet, so a master without wallets turns it off first,
 * `module casino off`. install() declares `casino-rooms`, `casino-seats`,
 * `casino-betting-seconds`, `casino-spinning-seconds` and
 * `casino-result-seconds`, read at each request; start() starts the
 * CasinoRoundJob on its thread CasinoClock, which draws each round with the
 * settings read then, and stop() stops it. The module provides nothing.
 */
class CasinoModule final : public engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Declares the settings of the casino and adds its routes; opens
   * nothing. */
  void install(engine::InstallContext& context) override;

  /** @brief Starts CasinoClock, which draws the rounds due at once, then
   * every CASINO_ROUND_INTERVAL. */
  void start() override;

  /** @brief Stops CasinoClock, once its pass in progress is done. */
  void stop() override;

 private:
  void declareSettings(engine::ConfigurationRegistry& configuration);

  /** @returns The settings of the casino, as the configuration holds them. */
  [[nodiscard]] CasinoSettings settings() const;

  const engine::SettingValue* rooms_{nullptr};
  const engine::SettingValue* seats_{nullptr};
  const engine::SettingValue* bettingSeconds_{nullptr};
  const engine::SettingValue* spinningSeconds_{nullptr};
  const engine::SettingValue* resultSeconds_{nullptr};
  network::SystemRouletteDraw draw_;
  std::optional<CasinoService> service_;
  std::optional<CasinoController> controller_;
  std::optional<RouletteDealer> dealer_;
  std::optional<CasinoRoundJob> roundJob_;
};

}  // namespace rtype::master
