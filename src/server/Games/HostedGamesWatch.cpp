#include "HostedGamesWatch.hpp"
#include <string>
#include <utility>
#include "EventBus.hpp"
#include "GameStateChanged.hpp"
#include "Games.hpp"
#include "HostedGames.hpp"
#include "LatestHostedGames.hpp"
#include "PlayerArrival.hpp"
#include "PlayerDeparture.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

HostedGamesWatch::HostedGamesWatch(LatestHostedGames& latest,
                                   std::string difficulty)
    : latest_(latest), difficulty_(std::move(difficulty)) {}

void HostedGamesWatch::watch(engine::EventBus& events) {
  subscriptions_.emplace_back(
      &events, events.subscribe<game::PlayerArrival>(
                   [this](const game::PlayerArrival&) { changed_ = true; }));
  subscriptions_.emplace_back(
      &events, events.subscribe<game::PlayerDeparture>(
                   [this](const game::PlayerDeparture&) { changed_ = true; }));
  subscriptions_.emplace_back(
      &events, events.subscribe<GameStateChanged>(
                   [this](const GameStateChanged&) { changed_ = true; }));
}

void HostedGamesWatch::publishIfChanged(const Games& games) {
  if (!changed_) {
    return;
  }
  changed_ = false;
  latest_.publish(hostedGamesOf(games, difficulty_));
}

void HostedGamesWatch::forget() {
  for (const auto& [events, handle] : subscriptions_) {
    static_cast<void>(events->unsubscribe(handle));
  }
  subscriptions_.clear();
}

}  // namespace rtype::server
