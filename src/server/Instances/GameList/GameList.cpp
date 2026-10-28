#include "GameList.hpp"
#include <memory>
#include <span>
#include <utility>
#include "GameInstance.hpp"
#include "ServerException.hpp"

namespace rtype::server {

void GameList::add(std::unique_ptr<GameInstance> game) {
  games_.push_back(std::move(game));
}

GameInstance& GameList::joinable() const {
  if (games_.empty()) {
    throw NoGameException();
  }
  return *games_.front();
}

std::span<const std::unique_ptr<GameInstance>> GameList::all() const {
  return games_;
}

void GameList::clear() {
  while (!games_.empty()) {
    games_.pop_back();
  }
}

}  // namespace rtype::server
