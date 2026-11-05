#include "ServerInformationModule.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "EngineException.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "IServerName.hpp"
#include "InformationConstants.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "OccupancyCount.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerConstants.hpp"
#include "ServerInformationDeskParts.hpp"
#include "TextSettingDefinition.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::server {

namespace {

void requireServerName(std::string_view name) {
  if (!network::isValidServerName(name)) {
    throw engine::CommandRefusedException(
        std::string{INVALID_SERVER_NAME_TEXT});
  }
}

}  // namespace

std::string ServerInformationModule::name() const {
  return std::string{INFORMATION_MODULE_NAME};
}

std::vector<std::string> ServerInformationModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}, std::string{GAMES_MODULE_NAME}};
}

void ServerInformationModule::install(engine::InstallContext& context) {
  declareSettings(context.require<engine::ConfigurationRegistry>());
  games_ = &context.require<Games>();
  sender_ = &context.require<IDatagramSender>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  context.addWork(SIMULATION_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    answerInformationRequests();
                  });
  context.provide<IServerName>(*this);
}

void ServerInformationModule::declareSettings(
    engine::ConfigurationRegistry& configuration) {
  nameSetting_ = &configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{SERVER_NAME_SETTING_NAME},
      .defaultValue = std::string{DEFAULT_SERVER_NAME},
      .help = "Name players read in the list of servers",
      .validate = requireServerName});
}

void ServerInformationModule::start() {
  IDatagramSender* const sender = sender_;
  const Games* const games = games_;
  desk_.emplace(ServerInformationDeskParts{
      .name = serverName(),
      .gameVersion = std::string{SERVER_GAME_VERSION},
      .send =
          [sender](network::OutgoingDatagram datagram) {
            sender->queueDatagram(std::move(datagram));
          },
      .occupancy = [games] { return occupancyOf(*games); }});
}

void ServerInformationModule::stop() { desk_.reset(); }

std::string ServerInformationModule::serverName() const {
  return nameSetting_->text();
}

void ServerInformationModule::answerInformationRequests() {
  if (desk_.has_value()) {
    desk_->answer(tickDatagrams_->informationRequests(), clock_.now());
  }
}

}  // namespace rtype::server
