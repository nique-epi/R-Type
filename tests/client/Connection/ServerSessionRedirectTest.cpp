#include <gtest/gtest.h>
#include <cstddef>
#include "ConnectionChallenge.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "ServerSessionFixture.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionRedirectTestConstants.hpp"
#include "ServerSessionTestConstants.hpp"

namespace rtype::client {

using testing::AFTER_THE_REDIRECT;
using testing::BEFORE_THE_REDIRECT;
using testing::CHALLENGE_COOKIE;
using testing::GAME_PORT;
using testing::NAMED_GAME_NUMBER;
using testing::OTHER_GAME_PORT;
using testing::server;
using testing::ServerSessionFixture;

/**
 * Given an attempt in progress
 * When the server redirects it to a game port
 * Then the session listens to that port at the address of the server, and the
 * request goes there at once
 */
TEST_F(ServerSessionFixture, FollowsARedirectToTheGamePortOfTheSameAddress) {
  connect();
  const network::Endpoint game{.address = server().address, .port = GAME_PORT};

  deliver(network::ConnectionRedirect{.port = GAME_PORT});
  update();

  EXPECT_EQ(listenedTo(), game);
  EXPECT_EQ(sent().back().destination, game);
}

/**
 * Given an attempt that got a challenge and answered it with its cookie
 * When the server redirects it
 * Then the request goes to the game without a cookie
 */
TEST_F(ServerSessionFixture, AsksTheGameWithoutTheCookieOfTheSupervisor) {
  connect();
  deliver(network::ConnectionChallenge{.cookie = CHALLENGE_COOKIE});
  update();

  deliver(network::ConnectionRedirect{.port = GAME_PORT});
  update();

  EXPECT_EQ(lastRequest().cookie, 0U);
}

/**
 * Given an attempt that waited four seconds
 * When the server redirects it, then one more second passes
 * Then the attempt timed out: the redirect did not restart its delay
 */
TEST_F(ServerSessionFixture, ARedirectDoesNotRestartTheDelayOfTheAttempt) {
  connect();
  update(BEFORE_THE_REDIRECT);
  deliver(network::ConnectionRedirect{.port = GAME_PORT});
  update();

  update(AFTER_THE_REDIRECT);

  EXPECT_EQ(phase(), ConnectionPhase::Unreachable);
}

/**
 * Given an attempt that was redirected to a game
 * When a second redirect names another port
 * Then it is ignored and the session keeps listening to the first game
 */
TEST_F(ServerSessionFixture, IgnoresASecondRedirectOfTheSameAttempt) {
  connect();
  deliver(network::ConnectionRedirect{.port = GAME_PORT});
  update();

  deliver(network::ConnectionRedirect{.port = OTHER_GAME_PORT});
  update();

  EXPECT_EQ(listenedTo().value_or(server()).port, GAME_PORT);
}

/**
 * Given a session that was accepted
 * When a redirect arrives
 * Then it is ignored and the session still listens to its server
 */
TEST_F(ServerSessionFixture, IgnoresARedirectOnceAccepted) {
  connectAndGetAccepted();

  deliverInSession(network::ConnectionRedirect{.port = GAME_PORT}, 1);
  update();

  EXPECT_EQ(listenedTo(), server());
}

/**
 * Given an attempt in progress
 * When a redirect names the port the session already uses
 * Then it is ignored and no request is sent
 */
TEST_F(ServerSessionFixture, IgnoresARedirectToThePortInUse) {
  connect();
  const std::size_t requestsBefore = sent().size();

  deliver(network::ConnectionRedirect{.port = server().port});
  update();

  EXPECT_EQ(sent().size(), requestsBefore);
}

/**
 * Given an attempt that was redirected
 * When the player connects again and the server redirects the new attempt
 * Then the new redirect is followed
 */
TEST_F(ServerSessionFixture, FollowsARedirectOfTheNextAttempt) {
  connect();
  deliver(network::ConnectionRedirect{.port = GAME_PORT});
  update();
  connect();

  deliver(network::ConnectionRedirect{.port = OTHER_GAME_PORT});
  update();

  EXPECT_EQ(listenedTo().value_or(server()).port, OTHER_GAME_PORT);
}

/**
 * Given a connection with no attempt
 * When it connects to a server asking for game 3
 * Then the request it sends at once asks for game 3
 */
TEST_F(ServerSessionFixture, SendsTheGameItAsksFor) {
  const network::GameChoice game{.kind = network::GameChoiceKind::ExistingGame,
                                 .number = NAMED_GAME_NUMBER};

  connect(game);

  EXPECT_EQ(lastRequest().game, game);
}

/**
 * Given an attempt that asks for a new game
 * When the supervisor redirects it to a game port
 * Then the request it sends there still asks for a new game
 */
TEST_F(ServerSessionFixture, KeepsTheGameItAsksForAfterARedirect) {
  connect(network::NEW_GAME_CHOICE);

  deliver(network::ConnectionRedirect{.port = GAME_PORT});
  update();

  EXPECT_EQ(sent().back().destination.port, GAME_PORT);
  EXPECT_EQ(lastRequest().game, network::NEW_GAME_CHOICE);
}

}  // namespace rtype::client
