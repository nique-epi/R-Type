#include "DeparturesFixture.hpp"
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "PlayerDeparture.hpp"
#include "SessionsFixtures.hpp"

namespace rtype::server::testing {

DeparturesFixture::DeparturesFixture()
    : player_(&addPlayer(roster_)),
      departures_(roster_, events_,
                  [this](const ConnectedPlayer& closing) {
                    closingPlayers_.push_back(closing);
                    seatFreedWhenClosing_ =
                        roster_.find(closing.endpoint) == nullptr;
                    events_.publish(game::EntityDestruction{
                        .networkId = closing.shipEntityId,
                        .cause = game::DestructionCause::Removed});
                  }),
      departureSubscription_(events_.subscribe<game::PlayerDeparture>(
          [this](const game::PlayerDeparture& departure) {
            announced_.push_back(departure);
            steps_.push_back(ClosingStep::DepartureAnnounced);
          })),
      destructionSubscription_(events_.subscribe<game::EntityDestruction>(
          [this](const game::EntityDestruction&) {
            steps_.push_back(ClosingStep::ShipRemoved);
          })) {}

DeparturesFixture::~DeparturesFixture() {
  events_.unsubscribe(departureSubscription_);
  events_.unsubscribe(destructionSubscription_);
}

PlayerRoster& DeparturesFixture::roster() { return roster_; }

Departures& DeparturesFixture::departures() { return departures_; }

ConnectedPlayer& DeparturesFixture::player() { return *player_; }

const std::vector<ConnectedPlayer>& DeparturesFixture::closingPlayers() const {
  return closingPlayers_;
}

bool DeparturesFixture::seatFreedWhenClosing() const {
  return seatFreedWhenClosing_;
}

const std::vector<game::PlayerDeparture>& DeparturesFixture::announced() {
  events_.dispatch();
  return announced_;
}

const std::vector<ClosingStep>& DeparturesFixture::steps() {
  events_.dispatch();
  return steps_;
}

}  // namespace rtype::server::testing
