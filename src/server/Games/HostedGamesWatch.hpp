#pragma once

#include <string>
#include <utility>
#include <vector>
#include "EventBus.hpp"
#include "Games.hpp"
#include "LatestHostedGames.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

/**
 * @brief Watches the bus of each game for what changes its line in the list of
 * hosted games (a player arriving or leaving, the game changing state), and
 * publishes the whole list again once the bus delivered such an event.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class HostedGamesWatch {
 public:
  /**
   * @param latest Where the list is published; it must outlive the watch.
   * @param difficulty The difficulty every game plays.
   */
  HostedGamesWatch(LatestHostedGames& latest, std::string difficulty);

  /** @brief Subscribes to the events of @p events that change a game. */
  void watch(engine::EventBus& events);

  /**
   * @brief Publishes the list of @p games when an event watched changed it
   * since the last call, and the first time it is called.
   */
  void publishIfChanged(const Games& games);

  /** @brief Unsubscribes from every bus watch() subscribed to. */
  void forget();

 private:
  /** @brief Marks the list changed each time @p events delivers an Event. */
  template <typename Event>
  void markChangedOn(engine::EventBus& events) {
    subscriptions_.emplace_back(
        &events,
        events.subscribe<Event>([this](const Event&) { changed_ = true; }));
  }

  LatestHostedGames& latest_;
  std::string difficulty_;
  bool changed_{true};
  std::vector<std::pair<engine::EventBus*, engine::SubscriptionHandle>>
      subscriptions_;
};

}  // namespace rtype::server
