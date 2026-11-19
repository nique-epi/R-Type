#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include "LobbyMapVoteFixture.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "ProtocolConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::testing::FIRST_MAP_NUMBER;
using rtype::client::testing::FIRST_VOTED_MAP;
using rtype::client::testing::MAP_BAR_MIDDLE;
using rtype::client::testing::NO_MAP_BAR;
using rtype::client::testing::OTHER_NICKNAME;
using rtype::client::testing::OTHER_NUMBER;
using rtype::client::testing::OWN_NICKNAME;
using rtype::client::testing::OWN_NUMBER;
using rtype::client::testing::SECOND_MAP_BAR;
using rtype::client::testing::SECOND_MAP_NUMBER;
using rtype::client::testing::SECOND_MAP_VOTE_LINE;
using rtype::client::testing::SECOND_VOTED_MAP;
using rtype::network::LobbyPlayer;
using rtype::network::NO_MAP_VOTE;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a room of two maps where another pilot voted for the second, and the
 * local player has not voted
 * When it is painted
 * Then the MAP bar shows the second map, the one leading the vote
 */
TEST_F(LobbyMapVoteFixture, TheBarShowsTheLeadingMapBeforeAnyVote) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE},
        LobbyPlayer{.playerNumber = OTHER_NUMBER,
                    .ready = false,
                    .nickname = OTHER_NICKNAME,
                    .mapVote = SECOND_MAP_NUMBER}},
       {FIRST_VOTED_MAP, SECOND_VOTED_MAP});

  EXPECT_NE(writingOf(canvas(), SECOND_MAP_BAR), nullptr);
}

/**
 * Given a room of two maps where nobody voted
 * When MAP_KEY is pressed then released
 * Then the room asks to vote for the first map, the one the bar shows
 */
TEST_F(LobbyMapVoteFixture, TheKeyVotesForTheMapShown) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       {FIRST_VOTED_MAP, SECOND_VOTED_MAP});

  pressMapKey(false);

  EXPECT_THAT(votes(), ElementsAre(FIRST_MAP_NUMBER));
}

/**
 * Given a room of two maps where the local player voted for the second
 * When MAP_KEY is pressed then released
 * Then the room asks to vote for the first, wrapping past the last map
 */
TEST_F(LobbyMapVoteFixture, AnotherPressVotesForTheNextMapWrapping) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       {FIRST_VOTED_MAP, SECOND_VOTED_MAP});
  pressMapKey(false);
  pressMapKey(false);

  pressMapKey(false);

  EXPECT_THAT(votes(), ElementsAre(FIRST_MAP_NUMBER, SECOND_MAP_NUMBER,
                                   FIRST_MAP_NUMBER));
}

/**
 * Given a room of two maps
 * When MAP_KEY is pressed twice with no release in between, as a key held
 * down repeats its press
 * Then the room asks for one vote
 */
TEST_F(LobbyMapVoteFixture, AHeldKeyVotesOnce) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       {FIRST_VOTED_MAP, SECOND_VOTED_MAP});

  pressMapKey(true);
  pressMapKey(true);

  EXPECT_THAT(votes(), ElementsAre(FIRST_MAP_NUMBER));
}

/**
 * Given a room of two maps
 * When the MAP bar is clicked
 * Then the room asks to vote for the map it shows
 */
TEST_F(LobbyMapVoteFixture, AClickOnTheBarVotes) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       {FIRST_VOTED_MAP, SECOND_VOTED_MAP});

  click(MAP_BAR_MIDDLE);

  EXPECT_THAT(votes(), ElementsAre(FIRST_MAP_NUMBER));
}

/**
 * Given a room that lists no map, as a server that names its level
 * When MAP_KEY is pressed then released
 * Then the bar says so, and the room asks for no vote
 */
TEST_F(LobbyMapVoteFixture, ARoomWithoutMapsVotesForNothing) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       {});

  pressMapKey(false);

  EXPECT_NE(writingOf(canvas(), NO_MAP_BAR), nullptr);
  EXPECT_THAT(votes(), IsEmpty());
}

/**
 * Given a room of two maps where another pilot voted for the second
 * When it is painted
 * Then the bay of that pilot writes its vote
 */
TEST_F(LobbyMapVoteFixture, ABayWritesTheVoteOfItsPilot) {
  open({LobbyPlayer{.playerNumber = OTHER_NUMBER,
                    .ready = false,
                    .nickname = OTHER_NICKNAME,
                    .mapVote = SECOND_MAP_NUMBER}},
       {FIRST_VOTED_MAP, SECOND_VOTED_MAP});

  EXPECT_NE(writingOf(canvas(), SECOND_MAP_VOTE_LINE), nullptr);
}
