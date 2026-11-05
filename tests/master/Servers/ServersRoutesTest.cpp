#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
#include "HostedGameState.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "ServerHeartbeat.hpp"
#include "ServersRoutesFixture.hpp"
#include "ServersRoutesTestConstants.hpp"

using rtype::network::heartbeatText;
using rtype::network::HostedGameState;
using rtype::network::isWellFormedServerKey;
using rtype::network::registrationText;
using rtype::network::serverStatusOf;
using rtype::network::testing::ADDRESS_POINTER;
using rtype::network::testing::FIRST_PILOT;
using rtype::network::testing::FOURTH_PILOT;
using rtype::network::testing::parisHeartbeat;
using rtype::network::testing::parisRegistration;
using rtype::network::testing::PORT_AS_TEXT;
using rtype::network::testing::quoted;
using rtype::network::testing::registrationTextWith;
using rtype::network::testing::registrationTextWithout;
using rtype::network::testing::SECOND_PILOT;
using rtype::network::testing::THIRD_PILOT;
using rtype::network::testing::UDP_PORT_POINTER;

/**
 * Given a master
 * When a host creates a server
 * Then it gets 201, the Location of the server named by its id, and its key
 */
TEST_F(ServersRoutesFixture, ACreationAnswersWhereTheServerLivesAndItsKey) {
  const httplib::Result answer = createParis();

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_CREATED);
  const nlohmann::json created = nlohmann::json::parse(answer->body);
  EXPECT_EQ(answer->get_header_value(std::string{LOCATION_HEADER_NAME}),
            std::string{EXPECTED_SERVER_LOCATION_PREFIX} +
                std::to_string(created.at(ID_MEMBER_NAME).get<std::int64_t>()));
  EXPECT_TRUE(
      isWellFormedServerKey(created.at(KEY_MEMBER_NAME).get<std::string>()));
}

/**
 * Given a master
 * When a host creates a server without a name
 * Then it gets 400, whose detail names the name
 */
TEST_F(ServersRoutesFixture, ACreationWithoutANameNamesTheMissingMember) {
  const httplib::Result answer =
      send(RouteCall{.method = POST_METHOD,
                     .path = SERVERS_URL,
                     .key = {},
                     .body = std::string{EMPTY_OBJECT_BODY}});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  EXPECT_NE(nlohmann::json::parse(answer->body)
                .at(PROBLEM_DETAIL_MEMBER_NAME)
                .get<std::string>()
                .find(quoted(NAME_POINTER_TEXT)),
            std::string::npos);
}

/**
 * Given the loopback, which created the server of the fixture
 * When it creates four more, then a sixth one
 * Then the four are created and the sixth gets 429
 */
TEST_F(ServersRoutesFixture, ASixthCreationInAnHourIsTooManyRequests) {
  for (std::size_t creation = 1; creation < CREATIONS_PER_HOUR; ++creation) {
    const httplib::Result answer = createParis();
    ASSERT_TRUE(answer);
    ASSERT_EQ(answer->status, EXPECTED_CREATED);
  }

  const httplib::Result answer = createParis();

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a created server
 * When it registers with no key
 * Then it gets 401, asked for a Bearer key
 */
TEST_F(ServersRoutesFixture, ARegistrationWithoutAKeyIsAskedForOne) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_SERVER_URL,
                     .key = {},
                     .body = registrationText(parisRegistration())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
  EXPECT_EQ(answer->get_header_value(std::string{CHALLENGE_HEADER_NAME}),
            EXPECTED_CHALLENGE);
}

/**
 * Given a created server
 * When a server registers with a well-formed key the master never issued
 * Then it gets 401
 */
TEST_F(ServersRoutesFixture, ARegistrationWithAKeyNeverIssuedIsRefused) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_SERVER_URL,
                     .key = UNKNOWN_SERVER_KEY,
                     .body = registrationText(parisRegistration())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a registration whose port is written as text, sent with no key
 * When the master reads it
 * Then it gets 401: the key is checked before the body
 */
TEST_F(ServersRoutesFixture, TheKeyIsCheckedBeforeTheBody) {
  const httplib::Result answer = send(
      RouteCall{.method = PUT_METHOD,
                .path = OWN_SERVER_URL,
                .key = {},
                .body = registrationTextWith(UDP_PORT_POINTER, PORT_AS_TEXT)});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_UNAUTHORIZED);
}

/**
 * Given a created server
 * When it registers with its key
 * Then it gets 200 and its representation, under the id of its creation
 */
TEST_F(ServersRoutesFixture, ARegistrationAnswersTheServer) {
  const httplib::Result answer = registerParis();

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(ID_MEMBER_NAME)
                .get<std::int64_t>(),
            createdId());
}

/**
 * Given a registration that names no address
 * When it is sent from the loopback
 * Then the server is registered at the loopback, the address of the peer
 */
TEST_F(ServersRoutesFixture, ARegistrationWithoutAddressTakesThePeer) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_SERVER_URL,
                     .key = key(),
                     .body = registrationTextWithout(ADDRESS_POINTER)});

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(ADDRESS_MEMBER_NAME)
                .get<std::string>(),
            LOOPBACK_ADDRESS);
}

/**
 * Given a registration whose port is written as text
 * When it is sent with a valid key
 * Then it gets 400, whose detail names the port
 */
TEST_F(ServersRoutesFixture, AnInvalidRegistrationNamesItsFaultyMember) {
  const httplib::Result answer = send(
      RouteCall{.method = PUT_METHOD,
                .path = OWN_SERVER_URL,
                .key = key(),
                .body = registrationTextWith(UDP_PORT_POINTER, PORT_AS_TEXT)});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  EXPECT_NE(nlohmann::json::parse(answer->body)
                .at(PROBLEM_DETAIL_MEMBER_NAME)
                .get<std::string>()
                .find(quoted(UDP_PORT_POINTER)),
            std::string::npos);
}

/**
 * Given a created server that has not registered
 * When its heartbeat comes
 * Then it gets 409
 */
TEST_F(ServersRoutesFixture, AHeartbeatBeforeTheRegistrationIsAConflict) {
  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_HEARTBEAT_URL,
                     .key = key(),
                     .body = heartbeatText(parisHeartbeat())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_CONFLICT);
}

/**
 * Given a registered server
 * When its heartbeat comes
 * Then it gets 200 and the orders waiting for it, none yet
 */
TEST_F(ServersRoutesFixture, AHeartbeatGetsTheOrdersWaiting) {
  ASSERT_TRUE(registerParis());

  const httplib::Result answer =
      send(RouteCall{.method = PUT_METHOD,
                     .path = OWN_HEARTBEAT_URL,
                     .key = key(),
                     .body = heartbeatText(parisHeartbeat())});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body).at(COMMANDS_MEMBER_NAME),
            nlohmann::json::array());
}

/**
 * Given a registered server that sent the heartbeat of "Paris 1"
 * When anyone reads its games, with no key
 * Then it gets 200 and the games of that heartbeat, with their names, states
 * and numbers of players
 */
TEST_F(ServersRoutesFixture, TheGamesOfTheLastHeartbeatAreListedToAnyone) {
  ASSERT_TRUE(registerParis());
  ASSERT_TRUE(sendHeartbeat(parisHeartbeat()));

  const httplib::Result answer = readGames(std::to_string(createdId()));

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body),
            nlohmann::json::parse(PARIS_GAMES_BODY));
}

/**
 * Given a registered server that sent the heartbeat of "Paris 1", whose games
 * hold pilots
 * When its games are read
 * Then no nickname is in the answer
 */
TEST_F(ServersRoutesFixture, TheListOfGamesNamesNoPlayer) {
  ASSERT_TRUE(registerParis());
  ASSERT_TRUE(sendHeartbeat(parisHeartbeat()));

  const httplib::Result answer = readGames(std::to_string(createdId()));

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->body.find(FIRST_PILOT), std::string::npos);
  EXPECT_EQ(answer->body.find(SECOND_PILOT), std::string::npos);
}

/**
 * Given a registered server whose first game is full and whose second is over
 * When its games are read
 * Then the first is listed as full and the second as in game
 */
TEST_F(ServersRoutesFixture, AFullGameIsFullAndAFinishedGameIsInGame) {
  ASSERT_TRUE(registerParis());
  rtype::network::ServerHeartbeat heartbeat = parisHeartbeat();
  heartbeat.games.front().state = HostedGameState::Full;
  heartbeat.games.front().players = {
      std::string{FIRST_PILOT}, std::string{SECOND_PILOT},
      std::string{THIRD_PILOT}, std::string{FOURTH_PILOT}};
  heartbeat.games.back().state = HostedGameState::Finished;
  ASSERT_TRUE(sendHeartbeat(heartbeat));

  const httplib::Result answer = readGames(std::to_string(createdId()));

  ASSERT_TRUE(answer);
  const nlohmann::json games =
      nlohmann::json::parse(answer->body).at(GAMES_MEMBER_NAME);
  EXPECT_EQ(games.at(0).at(STATE_MEMBER_NAME).get<std::string>(),
            std::string{FULL_STATE_NAME});
  EXPECT_EQ(games.at(1).at(STATE_MEMBER_NAME).get<std::string>(),
            std::string{IN_GAME_STATE_NAME});
}

/**
 * Given a registered server whose first game is starting
 * When its games are read
 * Then that game is listed as in game
 */
TEST_F(ServersRoutesFixture, AStartingGameIsInGame) {
  ASSERT_TRUE(registerParis());
  rtype::network::ServerHeartbeat heartbeat = parisHeartbeat();
  heartbeat.games.front().state = HostedGameState::Starting;
  ASSERT_TRUE(sendHeartbeat(heartbeat));

  const httplib::Result answer = readGames(std::to_string(createdId()));

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(GAMES_MEMBER_NAME)
                .at(0)
                .at(STATE_MEMBER_NAME)
                .get<std::string>(),
            std::string{IN_GAME_STATE_NAME});
}

/**
 * Given a registered server that sent no heartbeat yet
 * When its games are read
 * Then it gets 200 and no game
 */
TEST_F(ServersRoutesFixture, AServerThatSentNoHeartbeatListsNoGame) {
  ASSERT_TRUE(registerParis());

  const httplib::Result answer = readGames(std::to_string(createdId()));

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body),
            nlohmann::json::parse(NO_GAMES_BODY));
}

/**
 * Given a server whose heartbeat listed two games
 * When its next heartbeat lists one
 * Then its games are the one of the next heartbeat
 */
TEST_F(ServersRoutesFixture, TheNextHeartbeatReplacesTheGames) {
  ASSERT_TRUE(registerParis());
  ASSERT_TRUE(sendHeartbeat(parisHeartbeat()));
  rtype::network::ServerHeartbeat oneGame = parisHeartbeat();
  oneGame.games.pop_back();
  ASSERT_TRUE(sendHeartbeat(oneGame));

  const httplib::Result answer = readGames(std::to_string(createdId()));

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body).at(GAMES_MEMBER_NAME).size(),
            1U);
}

/**
 * Given a server that sent a heartbeat
 * When it registers again, as it does when it restarts
 * Then it lists no game
 */
TEST_F(ServersRoutesFixture, ARegistrationEmptiesTheListOfGames) {
  ASSERT_TRUE(registerParis());
  ASSERT_TRUE(sendHeartbeat(parisHeartbeat()));
  ASSERT_TRUE(registerParis());

  const httplib::Result answer = readGames(std::to_string(createdId()));

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body),
            nlohmann::json::parse(NO_GAMES_BODY));
}

/**
 * Given a master with one server
 * When the games of an id no server has, of a text that is no number, of 0 and
 * of -1 are read
 * Then each gets 404
 */
TEST_F(ServersRoutesFixture, AnIdThatNamesNoServerIsNotFound) {
  for (const std::string_view serverId : {UNUSED_SERVER_ID, NOT_A_SERVER_ID,
                                          ZERO_SERVER_ID, NEGATIVE_SERVER_ID}) {
    const httplib::Result answer = readGames(serverId);

    ASSERT_TRUE(answer);
    EXPECT_EQ(answer->status, EXPECTED_NOT_FOUND) << serverId;
  }
}

/**
 * Given a registered server that sent the heartbeat of "Paris 1"
 * When anyone reads the list of servers, with no key
 * Then it gets 200 and that server, with its address, its versions and the
 * counts of its heartbeat, and a status (whose value the tests of the
 * service check: here the refresh runs on its own clock)
 */
TEST_F(ServersRoutesFixture, TheRegisteredServersAreListedToAnyone) {
  ASSERT_TRUE(registerParis());
  ASSERT_TRUE(sendHeartbeat(parisHeartbeat()));

  const httplib::Result answer = readServers(NO_QUERY);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  nlohmann::json listed =
      nlohmann::json::parse(answer->body).at(SERVERS_MEMBER_NAME).at(0);
  EXPECT_TRUE(serverStatusOf(listed.at(STATUS_MEMBER_NAME).get<std::string>())
                  .has_value());
  listed.erase(std::string{STATUS_MEMBER_NAME});
  nlohmann::json expected = nlohmann::json::parse(PARIS_LISTED_SERVER_BODY);
  expected[std::string{ID_MEMBER_NAME}] = createdId();
  EXPECT_EQ(listed, expected);
}

/**
 * Given a server created and never registered
 * When the list of servers is read
 * Then it is empty
 */
TEST_F(ServersRoutesFixture, AServerThatNeverRegisteredIsNotListed) {
  const httplib::Result answer = readServers(NO_QUERY);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(nlohmann::json::parse(answer->body),
            nlohmann::json::parse(NO_SERVERS_BODY));
}

/**
 * Given the list of servers
 * When it is read with a limit of 0, of 101 or of no number, or a negative
 * offset
 * Then each read gets 400, whose detail gives the bounds
 */
TEST_F(ServersRoutesFixture, APageOutOfItsBoundsIsRefused) {
  for (const std::string_view query :
       {LIMIT_ZERO_QUERY, LIMIT_OVER_HUNDRED_QUERY, LIMIT_NOT_A_NUMBER_QUERY,
        NEGATIVE_OFFSET_QUERY}) {
    SCOPED_TRACE(query);
    const httplib::Result answer = readServers(query);

    ASSERT_TRUE(answer);
    EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
    EXPECT_EQ(nlohmann::json::parse(answer->body)
                  .at(PROBLEM_DETAIL_MEMBER_NAME)
                  .get<std::string>(),
              EXPECTED_INVALID_PAGE_DETAIL);
  }
}

/**
 * Given a registered server that sent the heartbeat of "Paris 1"
 * When anyone reads its samples
 * Then it gets one sample, with the counts and the measures of that
 * heartbeat, and its date as a number of seconds
 */
TEST_F(ServersRoutesFixture, TheSamplesOfAServerAreListed) {
  ASSERT_TRUE(registerParis());
  ASSERT_TRUE(sendHeartbeat(parisHeartbeat()));

  const httplib::Result answer =
      readHistory(std::to_string(createdId()), SAMPLES_PATH_SUFFIX);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  const nlohmann::json samples =
      nlohmann::json::parse(answer->body).at(SAMPLES_MEMBER_NAME);
  ASSERT_EQ(samples.size(), 1U);
  EXPECT_EQ(samples.at(0).at(GAME_COUNT_MEMBER_NAME), PARIS_GAME_COUNT);
  EXPECT_EQ(samples.at(0).at(PLAYER_COUNT_MEMBER_NAME), PARIS_PLAYER_COUNT);
  EXPECT_EQ(samples.at(0).at(MEASURES_MEMBER_NAME),
            nlohmann::json::parse(PARIS_MEASURES_BODY));
  EXPECT_TRUE(samples.at(0).at(TAKEN_AT_MEMBER_NAME).is_number_integer());
}

/**
 * Given no server of id 999, and an id that is no number
 * When their samples or their changes of status are read
 * Then each read gets 404
 */
TEST_F(ServersRoutesFixture, TheHistoryOfAnUnknownServerIsNotFound) {
  for (const std::string_view part :
       {SAMPLES_PATH_SUFFIX, STATUS_CHANGES_PATH_SUFFIX}) {
    SCOPED_TRACE(part);
    const httplib::Result unused = readHistory(UNUSED_SERVER_ID, part);
    const httplib::Result notANumber = readHistory(NOT_A_SERVER_ID, part);

    ASSERT_TRUE(unused);
    ASSERT_TRUE(notANumber);
    EXPECT_EQ(unused->status, EXPECTED_NOT_FOUND);
    EXPECT_EQ(notANumber->status, EXPECTED_NOT_FOUND);
  }
}

/**
 * Given a registered server
 * When anyone reads its changes of status
 * Then it gets 200 and a list (whose items the tests of the service check)
 */
TEST_F(ServersRoutesFixture, TheChangesOfStatusOfAServerAreListed) {
  ASSERT_TRUE(registerParis());

  const httplib::Result answer =
      readHistory(std::to_string(createdId()), STATUS_CHANGES_PATH_SUFFIX);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_TRUE(nlohmann::json::parse(answer->body)
                  .at(STATUS_CHANGES_MEMBER_NAME)
                  .is_array());
}
