#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "ProtocolTestConstants.hpp"
#include "ServerCreation.hpp"
#include "ServerRegistration.hpp"

using rtype::network::CreatedServer;
using rtype::network::createdServerFromText;
using rtype::network::createdServerText;
using rtype::network::heartbeatFromText;
using rtype::network::heartbeatText;
using rtype::network::isWellFormedServerKey;
using rtype::network::registrationFromText;
using rtype::network::registrationText;
using rtype::network::ServerCreation;
using rtype::network::serverCreationFromText;
using rtype::network::serverCreationText;
using rtype::network::ServerRegistration;
using rtype::network::testing::ADDRESS_POINTER;
using rtype::network::testing::AVERAGE_PING_POINTER;
using rtype::network::testing::createdServerRefusal;
using rtype::network::testing::FIFTH_PILOT;
using rtype::network::testing::FIRST_GAME_FIRST_PLAYER_POINTER;
using rtype::network::testing::FIRST_GAME_PLAYERS_POINTER;
using rtype::network::testing::FIRST_GAME_STATE_POINTER;
using rtype::network::testing::FIRST_PILOT;
using rtype::network::testing::FIVE_PLAYERS;
using rtype::network::testing::FOURTH_PILOT;
using rtype::network::testing::GAMES_ABOVE_THE_MOST;
using rtype::network::testing::GAMES_POINTER;
using rtype::network::testing::heartbeatRefusal;
using rtype::network::testing::heartbeatTextOfGames;
using rtype::network::testing::heartbeatTextWith;
using rtype::network::testing::HOST_NAME_ADDRESS;
using rtype::network::testing::KEY_POINTER;
using rtype::network::testing::KEY_WITHOUT_PREFIX;
using rtype::network::testing::LOSS_ABOVE_ALL;
using rtype::network::testing::LOSS_PERCENT_POINTER;
using rtype::network::testing::MAXIMUM_GAMES_POINTER;
using rtype::network::testing::MAXIMUM_PLAYERS_PER_GAME_POINTER;
using rtype::network::testing::NAME_OF_32_BYTES;
using rtype::network::testing::NAME_OF_33_BYTES;
using rtype::network::testing::NAME_POINTER;
using rtype::network::testing::NAME_WITH_LINE_FEED;
using rtype::network::testing::NEGATIVE_MEASURE;
using rtype::network::testing::NO_GAME;
using rtype::network::testing::NOT_JSON;
using rtype::network::testing::PARIS_ID;
using rtype::network::testing::PARIS_KEY;
using rtype::network::testing::PARIS_NAME;
using rtype::network::testing::parisHeartbeat;
using rtype::network::testing::parisRegistration;
using rtype::network::testing::PAUSED_STATE;
using rtype::network::testing::PORT_ABOVE_THE_RANGE;
using rtype::network::testing::PORT_AS_TEXT;
using rtype::network::testing::PORT_BELOW_THE_RANGE;
using rtype::network::testing::quoted;
using rtype::network::testing::QUOTED_WHOLE_MESSAGE;
using rtype::network::testing::registrationRefusal;
using rtype::network::testing::registrationTextWith;
using rtype::network::testing::registrationTextWithout;
using rtype::network::testing::SECOND_GAME_NUMBER_POINTER;
using rtype::network::testing::SECOND_PILOT;
using rtype::network::testing::SEVENTEEN_BYTE_NICKNAME;
using rtype::network::testing::SHORT_KEY;
using rtype::network::testing::THIRD_PILOT;
using rtype::network::testing::TICK_P50_POINTER;
using rtype::network::testing::UDP_PORT_POINTER;
using rtype::network::testing::UNKNOWN_MEMBER_POINTER;
using rtype::network::testing::UNKNOWN_MEMBER_VALUE;
using rtype::network::testing::UPPERCASE_KEY;
using rtype::network::testing::WAITING_GAME_NUMBER;
using ::testing::HasSubstr;
using ::testing::IsEmpty;

/**
 * Given the registration of a server that knows its address
 * When it is written as JSON and read back
 * Then every member comes back as it was
 */
TEST(MasterMessageText, ReadsBackAWrittenRegistration) {
  EXPECT_EQ(registrationFromText(registrationText(parisRegistration())),
            parisRegistration());
}

/**
 * Given the registration of a server that does not know its address
 * When it is written as JSON and read back
 * Then it still has no address
 */
TEST(MasterMessageText, KeepsAnUnknownAddressEmpty) {
  ServerRegistration registration = parisRegistration();
  registration.address.reset();

  EXPECT_EQ(registrationFromText(registrationText(registration)).address,
            std::nullopt);
}

/**
 * Given a heartbeat of two games and every measure but the ping
 * When it is written as JSON and read back
 * Then its games and measures come back as they were, the ping still empty
 */
TEST(MasterMessageText, ReadsBackAWrittenHeartbeat) {
  EXPECT_EQ(heartbeatFromText(heartbeatText(parisHeartbeat())),
            parisHeartbeat());
}

/**
 * Given a heartbeat whose ping is not measured
 * When it is written as JSON
 * Then the ping is left out, never sent as 0
 */
TEST(MasterMessageText, LeavesOutAMeasureNotTaken) {
  EXPECT_EQ(nlohmann::json::parse(heartbeatText(parisHeartbeat()))
                .contains(nlohmann::json::json_pointer(
                    std::string{AVERAGE_PING_POINTER})),
            false);
}

/**
 * Given a registration carrying a member no message defines
 * When it is read
 * Then it is accepted, so a newer server still registers
 */
TEST(MasterMessageText, IgnoresAMemberItDoesNotKnow) {
  EXPECT_EQ(registrationFromText(registrationTextWith(UNKNOWN_MEMBER_POINTER,
                                                      UNKNOWN_MEMBER_VALUE)),
            parisRegistration());
}

/**
 * Given a text that is no JSON
 * When it is read as a registration
 * Then the refusal names the whole message
 */
TEST(MasterMessageText, RefusesATextThatIsNoJson) {
  EXPECT_THAT(registrationRefusal(NOT_JSON), HasSubstr(QUOTED_WHOLE_MESSAGE));
}

/**
 * Given a registration without its port
 * When it is read
 * Then the refusal names the port
 */
TEST(MasterMessageText, RefusesARegistrationWithoutItsPort) {
  EXPECT_THAT(registrationRefusal(registrationTextWithout(UDP_PORT_POINTER)),
              HasSubstr(quoted(UDP_PORT_POINTER)));
}

/**
 * Given a registration whose port is written as text
 * When it is read
 * Then the refusal names the port
 */
TEST(MasterMessageText, RefusesAPortWrittenAsText) {
  EXPECT_THAT(
      registrationRefusal(registrationTextWith(UDP_PORT_POINTER, PORT_AS_TEXT)),
      HasSubstr(quoted(UDP_PORT_POINTER)));
}

/**
 * Given registrations of port 0 and of port 65536
 * When each is read
 * Then each refusal names the port
 */
TEST(MasterMessageText, RefusesAPortOutsideTheUdpRange) {
  EXPECT_THAT(registrationRefusal(
                  registrationTextWith(UDP_PORT_POINTER, PORT_BELOW_THE_RANGE)),
              HasSubstr(quoted(UDP_PORT_POINTER)));
  EXPECT_THAT(registrationRefusal(
                  registrationTextWith(UDP_PORT_POINTER, PORT_ABOVE_THE_RANGE)),
              HasSubstr(quoted(UDP_PORT_POINTER)));
}

/**
 * Given a name of 32 bytes, the longest a server may have
 * When the registration is read
 * Then it is accepted
 */
TEST(MasterMessageText, AcceptsANameOf32Bytes) {
  EXPECT_THAT(
      registrationRefusal(registrationTextWith(NAME_POINTER, NAME_OF_32_BYTES)),
      IsEmpty());
}

/**
 * Given names of 0 and of 33 bytes, and a name holding a line feed
 * When each registration is read
 * Then each refusal names the name
 */
TEST(MasterMessageText, RefusesANameEmptyTooLongOrNotPrintable) {
  for (const std::string_view name :
       {std::string_view{}, NAME_OF_33_BYTES, NAME_WITH_LINE_FEED}) {
    EXPECT_THAT(registrationRefusal(registrationTextWith(NAME_POINTER, name)),
                HasSubstr(quoted(NAME_POINTER)));
  }
}

/**
 * Given registrations of no game, and of games of five players
 * When each is read
 * Then each refusal names the bound it breaks
 */
TEST(MasterMessageText, RefusesBoundsAServerCannotHave) {
  EXPECT_THAT(
      registrationRefusal(registrationTextWith(MAXIMUM_GAMES_POINTER, NO_GAME)),
      HasSubstr(quoted(MAXIMUM_GAMES_POINTER)));
  EXPECT_THAT(registrationRefusal(registrationTextWith(
                  MAXIMUM_PLAYERS_PER_GAME_POINTER, FIVE_PLAYERS)),
              HasSubstr(quoted(MAXIMUM_PLAYERS_PER_GAME_POINTER)));
}

/**
 * Given a registration whose address is a host name
 * When it is read
 * Then the refusal names the address, since only an IPv4 address is taken
 */
TEST(MasterMessageText, RefusesAnAddressThatIsNoIpv4Address) {
  EXPECT_THAT(registrationRefusal(
                  registrationTextWith(ADDRESS_POINTER, HOST_NAME_ADDRESS)),
              HasSubstr(quoted(ADDRESS_POINTER)));
}

/**
 * Given a heartbeat of 64 games, the most a server may host, then one of 65
 * When each is read
 * Then the first is accepted and the second refusal names the games
 */
TEST(MasterMessageText, RefusesMoreGamesThanAServerMayHost) {
  EXPECT_THAT(heartbeatRefusal(heartbeatTextOfGames(GAMES_ABOVE_THE_MOST - 1)),
              IsEmpty());
  EXPECT_THAT(heartbeatRefusal(heartbeatTextOfGames(GAMES_ABOVE_THE_MOST)),
              HasSubstr(quoted(GAMES_POINTER)));
}

/**
 * Given a heartbeat whose second game takes the number of the first
 * When it is read
 * Then the refusal names the number of the second game
 */
TEST(MasterMessageText, RefusesTwoGamesOfOneNumber) {
  EXPECT_THAT(heartbeatRefusal(heartbeatTextWith(SECOND_GAME_NUMBER_POINTER,
                                                 WAITING_GAME_NUMBER)),
              HasSubstr(quoted(SECOND_GAME_NUMBER_POINTER)));
}

/**
 * Given a heartbeat whose first game is in a state no game has
 * When it is read
 * Then the refusal names that state
 */
TEST(MasterMessageText, RefusesAStateNoGameHas) {
  EXPECT_THAT(heartbeatRefusal(
                  heartbeatTextWith(FIRST_GAME_STATE_POINTER, PAUSED_STATE)),
              HasSubstr(quoted(FIRST_GAME_STATE_POINTER)));
}

/**
 * Given a heartbeat whose first player has a nickname of 17 bytes
 * When it is read
 * Then the refusal names that player
 */
TEST(MasterMessageText, RefusesAPlayerWhoseNicknameIsTooLong) {
  EXPECT_THAT(heartbeatRefusal(heartbeatTextWith(
                  FIRST_GAME_FIRST_PLAYER_POINTER, SEVENTEEN_BYTE_NICKNAME)),
              HasSubstr(quoted(FIRST_GAME_FIRST_PLAYER_POINTER)));
}

/**
 * Given a game of four players, the most a game takes, then one of five
 * When each heartbeat is read
 * Then the first is accepted and the second refusal names the players
 */
TEST(MasterMessageText, RefusesAFifthPlayerInAGame) {
  EXPECT_THAT(heartbeatRefusal(heartbeatTextWith(
                  FIRST_GAME_PLAYERS_POINTER,
                  nlohmann::json::array(
                      {FIRST_PILOT, SECOND_PILOT, THIRD_PILOT, FOURTH_PILOT}))),
              IsEmpty());
  EXPECT_THAT(heartbeatRefusal(heartbeatTextWith(
                  FIRST_GAME_PLAYERS_POINTER,
                  nlohmann::json::array({FIRST_PILOT, SECOND_PILOT, THIRD_PILOT,
                                         FOURTH_PILOT, FIFTH_PILOT}))),
              HasSubstr(quoted(FIRST_GAME_PLAYERS_POINTER)));
}

/**
 * Given heartbeats whose median tick is negative, or whose loss is above 100 %
 * When each is read
 * Then each refusal names that measure
 */
TEST(MasterMessageText, RefusesAMeasureOutOfItsBounds) {
  EXPECT_THAT(
      heartbeatRefusal(heartbeatTextWith(TICK_P50_POINTER, NEGATIVE_MEASURE)),
      HasSubstr(quoted(TICK_P50_POINTER)));
  EXPECT_THAT(
      heartbeatRefusal(heartbeatTextWith(LOSS_PERCENT_POINTER, LOSS_ABOVE_ALL)),
      HasSubstr(quoted(LOSS_PERCENT_POINTER)));
}

/**
 * Given the creation of "Paris 1"
 * When it is written as JSON and read back
 * Then its name comes back as it was
 */
TEST(MasterMessageText, ReadsBackAWrittenCreation) {
  const ServerCreation creation{.name = std::string{PARIS_NAME}};

  EXPECT_EQ(serverCreationFromText(serverCreationText(creation)), creation);
}

/**
 * Given the answer to the creation of "Paris 1", with its id and its key
 * When it is written as JSON and read back
 * Then its id, name and key come back as they were
 */
TEST(MasterMessageText, ReadsBackAWrittenCreatedServer) {
  const CreatedServer created{.id = PARIS_ID,
                              .name = std::string{PARIS_NAME},
                              .key = std::string{PARIS_KEY}};

  EXPECT_EQ(createdServerFromText(createdServerText(created)), created);
}

/**
 * Given a key of the shape the master issues
 * When it is checked
 * Then it is well formed
 */
TEST(MasterMessageText, AcceptsAKeyOfTheShapeTheMasterIssues) {
  EXPECT_TRUE(isWellFormedServerKey(PARIS_KEY));
}

/**
 * Given keys without the prefix, in uppercase, or one digit short
 * When each is checked
 * Then none is well formed
 */
TEST(MasterMessageText, RefusesAKeyShapedAlmostRight) {
  for (const std::string_view key :
       {KEY_WITHOUT_PREFIX, UPPERCASE_KEY, SHORT_KEY}) {
    SCOPED_TRACE(key);
    EXPECT_FALSE(isWellFormedServerKey(key));
  }
}

/**
 * Given an answer to a creation whose key is not well formed
 * When it is read
 * Then the refusal names the key
 */
TEST(MasterMessageText, RefusesACreatedServerWithAMalformedKey) {
  const std::string text =
      createdServerText(CreatedServer{.id = PARIS_ID,
                                      .name = std::string{PARIS_NAME},
                                      .key = std::string{SHORT_KEY}});

  EXPECT_THAT(createdServerRefusal(text), HasSubstr(quoted(KEY_POINTER)));
}
