#include "GameBusesModule.hpp"
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "EventBus.hpp"
#include "IGameBuses.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"

GameBusesModule::GameBusesModule(std::size_t gameCount) {
  for (std::size_t game = 0; game < gameCount; ++game) {
    buses_.push_back(std::make_unique<rtype::engine::EventBus>());
  }
}

std::string GameBusesModule::name() const {
  return std::string{rtype::server::GAMES_MODULE_NAME};
}

std::vector<std::string> GameBusesModule::dependencies() const { return {}; }

void GameBusesModule::install(rtype::engine::InstallContext& context) {
  context.provide<rtype::server::IGameBuses>(*this);
}

void GameBusesModule::start() {}

void GameBusesModule::stop() {}

std::span<const std::unique_ptr<rtype::engine::EventBus>>
GameBusesModule::buses() const {
  return buses_;
}
