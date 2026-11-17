#include "WalletModule.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "ControllerRoute.hpp"
#include "GameTicketService.hpp"
#include "IAccountWallet.hpp"
#include "ISessionAccounts.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "KopecksConstants.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "Wallet.hpp"
#include "WalletModuleConstants.hpp"
#include "WalletService.hpp"

namespace rtype::master {

std::string WalletModule::name() const {
  return std::string{WALLET_MODULE_NAME};
}

std::vector<std::string> WalletModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{STORAGE_MODULE_NAME},
          std::string{HTTP_MODULE_NAME}, std::string{ACCOUNTS_MODULE_NAME}};
}

void WalletModule::install(engine::InstallContext& context) {
  declareSettings(context.require<engine::ConfigurationRegistry>());
  WalletService& service = service_.emplace(
      context.require<MasterDatabase>(), [this] { return startingWallet(); });
  context.provide<IAccountWallet>(service);
  auto& accounts = context.require<ISessionAccounts>();
  auto& server = context.require<MasterHttpServer>();
  addRoutes(server, WALLET_ROUTES, controller_.emplace(accounts, service));
  GameTicketService& tickets =
      tickets_.emplace(context.require<MasterDatabase>());
  addRoutes(server, GAME_TICKET_ROUTES,
            ticketController_.emplace(accounts, tickets));
}

void WalletModule::start() {}

void WalletModule::stop() {}

void WalletModule::declareSettings(
    engine::ConfigurationRegistry& configuration) {
  startingKopecks_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{WALLET_STARTING_KOPECKS_SETTING_NAME},
          .defaultValue = DEFAULT_STARTING_KOPECKS,
          .lowest = 0,
          .highest = network::MAXIMUM_KOPECKS,
          .help = "Kopecks a wallet holds when it opens, at the first read "
                  "of an account"});
  startingChips_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{WALLET_STARTING_CHIPS_SETTING_NAME},
          .defaultValue = DEFAULT_STARTING_CHIPS,
          .lowest = 0,
          .highest = network::MAXIMUM_CHIPS,
          .help = "Chips a wallet holds when it opens, at the first read of "
                  "an account"});
}

network::Wallet WalletModule::startingWallet() const {
  return network::Wallet{
      .kopecks = static_cast<std::uint32_t>(
          startingKopecks_->integer().value_or(DEFAULT_STARTING_KOPECKS)),
      .chips = static_cast<std::uint32_t>(
          startingChips_->integer().value_or(DEFAULT_STARTING_CHIPS))};
}

}  // namespace rtype::master
