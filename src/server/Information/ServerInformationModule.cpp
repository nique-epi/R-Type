#include "ServerInformationModule.hpp"
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "EngineException.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "IServerName.hpp"
#include "IncomingDatagram.hpp"
#include "InformationConstants.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "OccupancyCount.hpp"
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
  desk_.emplace(ServerInformationDeskParts{
      .name = serverName(),
      .gameVersion = std::string{SERVER_GAME_VERSION},
      .send = std::bind_front(&IDatagramSender::queueDatagram, sender_)});
}

void ServerInformationModule::stop() { desk_.reset(); }

std::string ServerInformationModule::serverName() const {
  return nameSetting_->text();
}

void ServerInformationModule::answerInformationRequests() {
  const std::span<const network::IncomingDatagram> requests =
      tickDatagrams_->informationRequests();
  if (desk_.has_value() && !requests.empty()) {
    desk_->answer(requests, occupancyOf(*games_), clock_.now());
  }
}

}  // namespace rtype::server
