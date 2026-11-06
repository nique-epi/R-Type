#include "GameEventsModule.hpp"
#include <string>
#include <vector>
#include "EventBus.hpp"
#include "IGameEvents.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"

std::string GameEventsModule::name() const {
  return std::string{rtype::server::GAMES_MODULE_NAME};
}

std::vector<std::string> GameEventsModule::dependencies() const { return {}; }

void GameEventsModule::install(rtype::engine::InstallContext& context) {
  context.provide<rtype::server::IGameEvents>(*this);
}

void GameEventsModule::start() {}

void GameEventsModule::stop() {}

rtype::engine::EventBus& GameEventsModule::events() { return events_; }
