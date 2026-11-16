#include "CasinoModule.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "CasinoController.hpp"
#include "CasinoModuleConstants.hpp"
#include "CasinoRoomConstants.hpp"
#include "CasinoRoundJobConstants.hpp"
#include "CasinoService.hpp"
#include "CasinoSettings.hpp"
#include "ConfigurationRegistry.hpp"
#include "ControllerRoute.hpp"
#include "IAccountWallet.hpp"
#include "ISessionAccounts.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "RouletteDealer.hpp"
#include "RouletteDurations.hpp"
#include "RouletteSchedule.hpp"
#include "RoundClock.hpp"
#include "SettingValue.hpp"

namespace rtype::master {

namespace {

/** @returns The value of @p setting, @p absent when it holds none. */
std::int64_t integerOf(const engine::SettingValue* setting,
                       std::int64_t absent) {
  return setting->integer().value_or(absent);
}

}  // namespace

std::string CasinoModule::name() const {
  return std::string{CASINO_MODULE_NAME};
}

std::vector<std::string> CasinoModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{STORAGE_MODULE_NAME},
          std::string{HTTP_MODULE_NAME}, std::string{ACCOUNTS_MODULE_NAME},
          std::string{WALLET_MODULE_NAME}};
}

void CasinoModule::install(engine::InstallContext& context) {
  declareSettings(context.require<engine::ConfigurationRegistry>());
  auto& database = context.require<MasterDatabase>();
  auto& wallet = context.require<IAccountWallet>();
  CasinoService& service =
      service_.emplace(database, wallet, [this] { return settings(); });
  dealer_.emplace(database, wallet, draw_);
  CasinoController& controller =
      controller_.emplace(context.require<ISessionAccounts>(), service);
  addRoutes(context.require<MasterHttpServer>(), CASINO_ROUTES, controller);
}

void CasinoModule::start() {
  roundJob_.emplace(
      *dealer_, settings(),
      RoundClock{.now = currentMillisecond, .interval = CASINO_ROUND_INTERVAL});
}

void CasinoModule::stop() { roundJob_.reset(); }

void CasinoModule::declareSettings(
    engine::ConfigurationRegistry& configuration) {
  rooms_ = &configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{CASINO_ROOMS_SETTING_NAME},
      .defaultValue = DEFAULT_CASINO_ROOMS,
      .lowest = FEWEST_CASINO_ROOMS,
      .highest = network::MAXIMUM_CASINO_ROOMS,
      .help = "Roulette rooms the casino runs"});
  seats_ = &configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{CASINO_SEATS_SETTING_NAME},
      .defaultValue = DEFAULT_CASINO_SEATS,
      .lowest = FEWEST_CASINO_SEATS,
      .highest = network::MAXIMUM_CASINO_SEATS,
      .help = "Players each room of the casino seats"});
  bettingSeconds_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{CASINO_BETTING_SECONDS_SETTING_NAME},
          .defaultValue = DEFAULT_BETTING_SECONDS,
          .lowest = SHORTEST_BETTING_SECONDS,
          .highest = LONGEST_BETTING_SECONDS,
          .help = "Seconds the bets of a round of roulette stay open"});
  spinningSeconds_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{CASINO_SPINNING_SECONDS_SETTING_NAME},
          .defaultValue = DEFAULT_SPINNING_SECONDS,
          .lowest = SHORTEST_SHOW_SECONDS,
          .highest = LONGEST_SHOW_SECONDS,
          .help = "Seconds the wheel spins once the bets close"});
  resultSeconds_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{CASINO_RESULT_SECONDS_SETTING_NAME},
          .defaultValue = DEFAULT_RESULT_SECONDS,
          .lowest = SHORTEST_SHOW_SECONDS,
          .highest = LONGEST_SHOW_SECONDS,
          .help = "Seconds the result shows before the next round"});
}

CasinoSettings CasinoModule::settings() const {
  return CasinoSettings{
      .rooms =
          static_cast<std::uint8_t>(integerOf(rooms_, DEFAULT_CASINO_ROOMS)),
      .seats =
          static_cast<std::uint8_t>(integerOf(seats_, DEFAULT_CASINO_SEATS)),
      .durations =
          RouletteDurations{.betting = std::chrono::seconds{integerOf(
                                bettingSeconds_, DEFAULT_BETTING_SECONDS)},
                            .spinning = std::chrono::seconds{integerOf(
                                spinningSeconds_, DEFAULT_SPINNING_SECONDS)},
                            .result = std::chrono::seconds{integerOf(
                                resultSeconds_, DEFAULT_RESULT_SECONDS)}}};
}

}  // namespace rtype::master
