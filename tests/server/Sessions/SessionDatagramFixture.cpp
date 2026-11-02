#include "SessionDatagramFixture.hpp"
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"
#include "SessionsFixtures.hpp"

namespace rtype::server::testing {

SessionDatagramFixture::SessionDatagramFixture()
    : firstPlayer_(&addPlayer(roster_)) {}

PlayerRoster& SessionDatagramFixture::roster() { return roster_; }

ConnectedPlayer& SessionDatagramFixture::firstPlayer() { return *firstPlayer_; }

}  // namespace rtype::server::testing
