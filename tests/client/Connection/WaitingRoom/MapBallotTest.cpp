#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "LobbyState.hpp"
#include "MapBallotFixture.hpp"
#include "MapBallotTestConstants.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client::testing {

using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given an open session
 * When the player votes for the second map
 * Then a MapVote for it goes out at once, and it is the map requested
 */
TEST_F(MapBallotFixture, AVoteGoesOutAtOnce) {
  open();

  ballot().voteFor(BALLOT_MAP);

  EXPECT_THAT(sentVotes(), ElementsAre(BALLOT_MAP));
  EXPECT_EQ(ballot().requestedMap().value_or(network::NO_MAP_VOTE), BALLOT_MAP);
}

/**
 * Given a vote the room does not show
 * When 499 ms pass, then one more
 * Then it goes out again only once the 500 ms passed
 */
TEST_F(MapBallotFixture, AVoteTheRoomDoesNotShowGoesOutAgain) {
  open();
  ballot().voteFor(BALLOT_MAP);

  ballot().sessionHeld(JUST_SHORT_OF_A_RESEND);
  const std::vector<std::uint8_t> beforeTheResend = sentVotes();
  ballot().sessionHeld(LAST_MILLISECOND);

  EXPECT_THAT(beforeTheResend, ElementsAre(BALLOT_MAP));
  EXPECT_THAT(sentVotes(), ElementsAre(BALLOT_MAP, BALLOT_MAP));
}

/**
 * Given a vote the room shows for the player
 * When 500 ms pass
 * Then it does not go out again
 */
TEST_F(MapBallotFixture, AVoteTheRoomShowsGoesOutOnce) {
  open();
  ballot().voteFor(BALLOT_MAP);
  room().setLobbyPlayers(
      {network::LobbyPlayer{.playerNumber = BALLOT_PLAYER_NUMBER,
                            .ready = false,
                            .nickname = std::string{BALLOT_PLAYER_NICKNAME},
                            .mapVote = BALLOT_MAP}});

  ballot().sessionHeld(JUST_SHORT_OF_A_RESEND + LAST_MILLISECOND);

  EXPECT_THAT(sentVotes(), ElementsAre(BALLOT_MAP));
}

/**
 * Given no session open
 * When the player votes
 * Then nothing goes out and no map is requested
 */
TEST_F(MapBallotFixture, NothingGoesOutWithoutASession) {
  ballot().voteFor(BALLOT_MAP);

  EXPECT_THAT(sentVotes(), IsEmpty());
  EXPECT_FALSE(ballot().requestedMap().has_value());
}

/**
 * Given an open session whose game started
 * When the player votes
 * Then nothing goes out
 */
TEST_F(MapBallotFixture, NothingGoesOutOnceTheGameStarted) {
  open();
  room().setGameStarted(true);

  ballot().voteFor(BALLOT_MAP);

  EXPECT_THAT(sentVotes(), IsEmpty());
}

/**
 * Given a vote of a session that then closed
 * When another session opens
 * Then no map is requested: the vote ended with its session
 */
TEST_F(MapBallotFixture, AClosedSessionForgetsItsVote) {
  open();
  ballot().voteFor(BALLOT_MAP);

  ballot().sessionClosed();
  open();

  EXPECT_FALSE(ballot().requestedMap().has_value());
}

}  // namespace rtype::client::testing
