#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Departures.hpp"
#include "EventBus.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerRoster.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server::testing {

/** @brief What the bus delivered for a closing session, in delivery order. */
enum class ClosingStep : std::uint8_t {
  ShipRemoved,
  DepartureAnnounced,
};

/**
 * @brief A roster with one player, and the Departures that close its session.
 * The function called for a closing player removes its ship the way a game
 * does: it publishes an EntityDestruction.
 */
class DeparturesFixture : public ::testing::Test {
 public:
  DeparturesFixture();
  DeparturesFixture(const DeparturesFixture&) = delete;
  DeparturesFixture& operator=(const DeparturesFixture&) = delete;
  DeparturesFixture(DeparturesFixture&&) = delete;
  DeparturesFixture& operator=(DeparturesFixture&&) = delete;
  ~DeparturesFixture() override;

 protected:
  [[nodiscard]] PlayerRoster& roster();
  [[nodiscard]] Departures& departures();
  [[nodiscard]] ConnectedPlayer& player();

  /** @returns The players the closing function was called with. */
  [[nodiscard]] const std::vector<ConnectedPlayer>& closingPlayers() const;

  /** @returns Whether the seat was already free when the closing function ran
   * for the last time. */
  [[nodiscard]] bool seatFreedWhenClosing() const;

  /** @returns The departures announced once the bus has delivered them. */
  [[nodiscard]] const std::vector<game::PlayerDeparture>& announced();

  /** @returns The steps the bus delivered, in order. */
  [[nodiscard]] const std::vector<ClosingStep>& steps();

 private:
  PlayerRoster roster_;
  engine::EventBus events_;
  ConnectedPlayer* player_;
  std::vector<ConnectedPlayer> closingPlayers_;
  bool seatFreedWhenClosing_{false};
  Departures departures_;
  std::vector<game::PlayerDeparture> announced_;
  std::vector<ClosingStep> steps_;
  engine::SubscriptionHandle departureSubscription_;
  engine::SubscriptionHandle destructionSubscription_;
};

}  // namespace rtype::server::testing
