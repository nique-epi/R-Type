#include "RunningGame.hpp"
#include <memory>
#include <utility>
#include "EntitySheets.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "ServerException.hpp"

namespace rtype::server {

void RunningGame::host(std::unique_ptr<GameInstance> game) {
  game_ = std::move(game);
}

GameInstance& RunningGame::game() const {
  if (game_ == nullptr) {
    throw GameNotCreatedException();
  }
  return *game_;
}

engine::EventBus& RunningGame::events() { return events_; }

void RunningGame::keepEntityTypes(const game::EntitySheets& types) {
  entityTypes_ = &types;
}

const game::EntitySheets* RunningGame::entityTypes() const {
  return entityTypes_;
}

}  // namespace rtype::server
