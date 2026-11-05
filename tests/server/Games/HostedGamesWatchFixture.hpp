#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include "EventBus.hpp"
#include "Games.hpp"
#include "HostedGamesWatch.hpp"
#include "LatestHostedGames.hpp"

namespace rtype::server::testing {

/** @brief A watch over one bus, publishing the list of no game. */
class HostedGamesWatchFixture : public ::testing::Test {
 protected:
  HostedGamesWatchFixture();

  /** @brief Publishes @p event on the bus, delivers it, then lets the watch
   * publish if it changed the list. */
  template <typename Event>
  void deliver(const Event& event) {
    events_.publish(event);
    events_.dispatch();
    watch_.publishIfChanged(games_);
  }

  [[nodiscard]] HostedGamesWatch& watch();
  [[nodiscard]] const Games& games() const;

  /** @returns How many lists the watch published. */
  [[nodiscard]] std::uint64_t listsPublished() const;

 private:
  engine::EventBus events_;
  Games games_;
  LatestHostedGames latest_;
  HostedGamesWatch watch_;
};

}  // namespace rtype::server::testing
