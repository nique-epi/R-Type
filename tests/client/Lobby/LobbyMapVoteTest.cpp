#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include "InterfaceColorConstants.hpp"
#include "LobbyMapVoteFixture.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "LobbyState.hpp"
#include "ProtocolConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::PLAYER_COLORS;
using rtype::client::testing::FIRST_MAP_NUMBER;
using rtype::client::testing::LEFT_OF_THE_PREVIEW;
using rtype::client::testing::MAP_BAR_MIDDLE;
using rtype::client::testing::NO_MAP_BAR;
using rtype::client::testing::NO_MAP_CHOSEN;
using rtype::client::testing::OTHER_NICKNAME;
using rtype::client::testing::OTHER_NUMBER;
using rtype::client::testing::OWN_NICKNAME;
using rtype::client::testing::OWN_NUMBER;
using rtype::client::testing::SECOND_MAP_BAR;
using rtype::client::testing::SECOND_MAP_NUMBER;
using rtype::client::testing::SECOND_MAP_PREVIEW;
using rtype::client::testing::SECOND_MAP_VOTE_LINE;
using rtype::client::testing::VOTE_PIP;
using rtype::network::FIRST_PLAYER_NUMBER;
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
       twoMaps());

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
       twoMaps());

  pressMapKey(false);

  EXPECT_THAT(votes(), ElementsAre(FIRST_MAP_NUMBER));
}

/**
 * Given a room of two maps where the local player voted for the second, the
 * last
 * When MAP_KEY is pressed then released
 * Then the room asks to vote for a random map, the choice after the last map
 */
TEST_F(LobbyMapVoteFixture, APressAfterTheLastMapVotesForARandomMap) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       twoMaps());
  pressMapKey(false);
  pressMapKey(false);

  pressMapKey(false);

  EXPECT_THAT(votes(),
              ElementsAre(FIRST_MAP_NUMBER, SECOND_MAP_NUMBER, NO_MAP_VOTE));
}

/**
 * Given a room of two maps where the local player voted for a random map
 * When MAP_KEY is pressed then released
 * Then the room asks to vote for the first map, wrapping around
 */
TEST_F(LobbyMapVoteFixture, APressAfterARandomVoteVotesForTheFirstMap) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       twoMaps());
  pressMapKey(false);
  pressMapKey(false);
  pressMapKey(false);

  pressMapKey(false);

  EXPECT_THAT(votes(), ElementsAre(FIRST_MAP_NUMBER, SECOND_MAP_NUMBER,
                                   NO_MAP_VOTE, FIRST_MAP_NUMBER));
}

/**
 * Given a room of two maps where another pilot voted for the second
 * When the local player votes for the second, then for a random map
 * Then the card names no map but a random one, not the map leading the vote
 */
TEST_F(LobbyMapVoteFixture, TheCardSaysRandomOnceThePlayerVotesForIt) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE},
        LobbyPlayer{.playerNumber = OTHER_NUMBER,
                    .ready = false,
                    .nickname = OTHER_NICKNAME,
                    .mapVote = SECOND_MAP_NUMBER}},
       twoMaps());
  pressMapKey(false);

  pressMapKey(false);

  EXPECT_THAT(votes(), ElementsAre(SECOND_MAP_NUMBER, NO_MAP_VOTE));
  EXPECT_NE(writingOf(canvas(), NO_MAP_CHOSEN), nullptr);
}

/**
 * Given a room of two maps where the local player voted for the first
 * When the card is clicked left of its preview
 * Then the room asks to vote for a random map, the choice before the first
 */
TEST_F(LobbyMapVoteFixture, BackFromTheFirstMapVotesForARandomMap) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       twoMaps());
  pressMapKey(false);

  click(LEFT_OF_THE_PREVIEW);

  EXPECT_THAT(votes(), ElementsAre(FIRST_MAP_NUMBER, NO_MAP_VOTE));
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
       twoMaps());

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
       twoMaps());

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
       twoMaps());

  EXPECT_NE(writingOf(canvas(), SECOND_MAP_VOTE_LINE), nullptr);
}

/**
 * Given a room of two maps where another pilot voted for the second
 * When it is painted
 * Then the card draws the preview of the second map, the one shown
 */
TEST_F(LobbyMapVoteFixture, TheCardDrawsThePreviewOfTheMapShown) {
  open({LobbyPlayer{.playerNumber = OTHER_NUMBER,
                    .ready = false,
                    .nickname = OTHER_NICKNAME,
                    .mapVote = SECOND_MAP_NUMBER}},
       twoMaps());

  EXPECT_THAT(previewsDrawn(), ElementsAre(SECOND_MAP_PREVIEW));
}

/**
 * Given a room of two maps where nobody voted
 * When it is painted
 * Then the card names no map but a random one, and draws no preview
 */
TEST_F(LobbyMapVoteFixture, TheCardSaysRandomWhileNobodyVoted) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       twoMaps());

  EXPECT_NE(writingOf(canvas(), NO_MAP_CHOSEN), nullptr);
  EXPECT_THAT(previewsDrawn(), IsEmpty());
}

/**
 * Given a room of two maps where nobody voted
 * When the card is clicked left of its preview
 * Then the room votes for the map before the first, the last one
 */
TEST_F(LobbyMapVoteFixture, AClickLeftOfThePreviewVotesForThePreviousMap) {
  open({LobbyPlayer{.playerNumber = OWN_NUMBER,
                    .ready = false,
                    .nickname = OWN_NICKNAME,
                    .mapVote = NO_MAP_VOTE}},
       twoMaps());

  click(LEFT_OF_THE_PREVIEW);

  EXPECT_THAT(votes(), ElementsAre(SECOND_MAP_NUMBER));
}

/**
 * Given a room of two maps where the pilot of the third bay voted for the
 * second
 * When it is painted
 * Then the card shows a pip of the ship color of that pilot
 */
TEST_F(LobbyMapVoteFixture, TheCardShowsAPipForEachVote) {
  open({LobbyPlayer{.playerNumber = OTHER_NUMBER,
                    .ready = false,
                    .nickname = OTHER_NICKNAME,
                    .mapVote = SECOND_MAP_NUMBER}},
       twoMaps());

  EXPECT_TRUE(hasFillOf(
      VOTE_PIP, PLAYER_COLORS[OTHER_NUMBER - FIRST_PLAYER_NUMBER].ship));
}
