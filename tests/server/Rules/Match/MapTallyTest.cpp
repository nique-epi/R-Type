#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "MapTallyFixtures.hpp"
#include "MapTallyTestConstants.hpp"

namespace rtype::server::testing {

using ::testing::ElementsAre;

/**
 * Given three maps, two players voting for the second and one for the first
 * When the vote is tallied with any seed
 * Then the second map is chosen, every time
 */
TEST(MapTally, ChoosesTheMapWithTheMostVotes) {
  const std::array votes{VOTE_FOR_THE_SECOND, VOTE_FOR_THE_FIRST,
                         VOTE_FOR_THE_SECOND};
  std::vector<ConnectedPlayer> players = votingPlayers(votes);

  EXPECT_THAT(mapsChosenOverSeeds(players, THREE_TALLIED_MAPS),
              ElementsAre(SECOND_MAP_INDEX));
}

/**
 * Given three maps, one player voting for the first and one for the third
 * When the vote is tallied with many seeds
 * Then the first and the third come out, never the second
 */
TEST(MapTally, DrawsAmongTheMapsTiedAtTheTop) {
  const std::array votes{VOTE_FOR_THE_FIRST, VOTE_FOR_THE_THIRD};
  std::vector<ConnectedPlayer> players = votingPlayers(votes);

  EXPECT_THAT(mapsChosenOverSeeds(players, THREE_TALLIED_MAPS),
              ElementsAre(FIRST_MAP_INDEX, THIRD_MAP_INDEX));
}

/**
 * Given three maps and two players who did not vote
 * When the vote is tallied with many seeds
 * Then every map comes out
 */
TEST(MapTally, DrawsAmongEveryMapWithoutVotes) {
  const std::array<std::uint8_t, 2> votes{};
  std::vector<ConnectedPlayer> players = votingPlayers(votes);

  EXPECT_THAT(mapsChosenOverSeeds(players, THREE_TALLIED_MAPS),
              ElementsAre(FIRST_MAP_INDEX, SECOND_MAP_INDEX, THIRD_MAP_INDEX));
}

/**
 * Given two maps, two away players voting for the first and one player for
 * the second
 * When the vote is tallied
 * Then the second map is chosen: a player who is away does not vote
 */
TEST(MapTally, CountsNoVoteOfAPlayerWhoIsAway) {
  const std::array votes{VOTE_FOR_THE_FIRST, VOTE_FOR_THE_FIRST,
                         VOTE_FOR_THE_SECOND};
  std::vector<ConnectedPlayer> players = votingPlayers(votes);
  players.at(0).away = true;
  players.at(1).away = true;

  EXPECT_EQ(mapChosen(players, TWO_TALLIED_MAPS), SECOND_MAP_INDEX);
}

/**
 * Given two maps, two players voting for a third map and one for the second
 * When the vote is tallied
 * Then the second map is chosen: a vote past the last map counts for none
 */
TEST(MapTally, CountsNoVotePastTheLastMap) {
  const std::array votes{VOTE_FOR_THE_THIRD, VOTE_FOR_THE_THIRD,
                         VOTE_FOR_THE_SECOND};
  std::vector<ConnectedPlayer> players = votingPlayers(votes);

  EXPECT_EQ(mapChosen(players, TWO_TALLIED_MAPS), SECOND_MAP_INDEX);
}

}  // namespace rtype::server::testing
