#include "Games.hpp"
#include <memory>
#include <span>
#include <utility>
#include "GameInstance.hpp"
#include "ServerException.hpp"

namespace rtype::server {

void Games::add(std::unique_ptr<GameInstance> game) {
  games_.push_back(std::move(game));
}

GameInstance& Games::joinable() const {
  if (games_.empty()) {
    throw NoJoinableGameException();
  }
  return *games_.front();
}

std::span<const std::unique_ptr<GameInstance>> Games::all() const {
  return games_;
}

}  // namespace rtype::server
