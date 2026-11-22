#include <gtest/gtest.h>
#include <string>
#include "Bounties.hpp"
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"
#include "ScoreKeeperTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given two connected players, the first ten kopecks short of the largest
 * bounty
 * When every player is paid the bounty of the Ash Mother, 550
 * Then the first stops at the largest bounty and the second holds 550
 */
TEST(Bounties, StopsABountyAtItsLargestValue) {
  PlayerRoster roster;
  ConnectedPlayer* const first = roster.add(
      playerEndpoint(0), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  ConnectedPlayer* const second = roster.add(
      playerEndpoint(1), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  first->bounty = LARGEST_BOUNTY - TEN_KOPECKS;

  payEveryPlayer(roster, ASH_MOTHER_BOUNTY);

  EXPECT_EQ(first->bounty, LARGEST_BOUNTY);
  EXPECT_EQ(second->bounty, ASH_MOTHER_BOUNTY);
}

}  // namespace rtype::server::testing
