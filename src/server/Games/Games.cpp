#include "Games.hpp"
#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GamePlacement.hpp"
#include "GameVacancy.hpp"
#include "ServerException.hpp"

namespace rtype::server {

void Games::add(std::unique_ptr<engine::EventBus> events,
                std::unique_ptr<GameInstance> game) {
  events_.push_back(std::move(events));
  games_.push_back(std::move(game));
}

GameInstance& Games::joinable() const { return *games_.at(joinableIndex()); }

std::size_t Games::placeOf(const network::Endpoint& sender) const {
  for (std::size_t index = 0; index < games_.size(); ++index) {
    if (games_[index]->roster().find(sender) != nullptr) {
      return index;
    }
  }
  return joinableIndex();
}

void Games::dispatchEvents() {
  for (const std::unique_ptr<engine::EventBus>& events : events_) {
    events->dispatch();
  }
}

std::span<const std::unique_ptr<GameInstance>> Games::all() const {
  return games_;
}

std::size_t Games::joinableIndex() const {
  if (games_.empty()) {
    throw NoJoinableGameException();
  }
  std::vector<GameVacancy> vacancies;
  vacancies.reserve(games_.size());
  for (const std::unique_ptr<GameInstance>& game : games_) {
    vacancies.push_back(
        GameVacancy{.state = game->state(), .full = game->roster().isFull()});
  }
  return firstJoinableGame(vacancies).value_or(0);
}

}  // namespace rtype::server
