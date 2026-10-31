#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include "ConnectionPhase.hpp"
#include "ConnectionRefusalFixture.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenChoiceFixture.hpp"
#include "ConnectionScreenConstants.hpp"
#include "ConnectionScreenFixture.hpp"
#include "ConnectionScreenFixtures.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenId.hpp"
#include "ScreenTestConstants.hpp"
#include "ServerConnectionTestConstants.hpp"
#include "SoundConstants.hpp"

using rtype::client::ACCEPTED_STATUS_DURATION;
using rtype::client::CLICK_SOUND_ID;
using rtype::client::CONNECTING_STATUS;
using rtype::client::ConnectionPhase;
using rtype::client::GAME_FULL_STATUS;
using rtype::client::GAME_IN_PROGRESS_STATUS;
using rtype::client::INVALID_ADDRESS_STATUS;
using rtype::client::INVALID_PORT_STATUS;
using rtype::client::MISSING_NICKNAME_STATUS;
using rtype::client::NICKNAME_TAKEN_STATUS;
using rtype::client::ScreenId;
using rtype::client::UNREACHABLE_STATUS;
using rtype::client::testing::ACCEPTED_AS_FIRST_PLAYER;
using rtype::client::testing::CANCEL;
using rtype::client::testing::LAST_ADDRESS_NUMBER_TEXT;
using rtype::client::testing::LAUNCH_ADDRESS;
using rtype::client::testing::LAUNCH_NICKNAME;
using rtype::client::testing::LAUNCH_PORT;
using rtype::client::testing::LAUNCH_PORT_TEXT;
using rtype::client::testing::NEXT_PROTOCOL_VERSION;
using rtype::client::testing::NO_NICKNAME_TEXT;
using rtype::client::testing::PORT_ZERO_TEXT;
using rtype::client::testing::THIRD_PLAYER;
using rtype::client::testing::TYPED_NICKNAME;
using rtype::client::testing::TYPED_NICKNAME_TEXT;
using rtype::client::testing::TYPED_PORT;
using rtype::client::testing::TYPED_PORT_TEXT;
using rtype::network::RefusalReason;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Not;

/**
 * Given the connection screen just built with the launch values
 * When it is painted
 * Then its fields read the address, the port and the nickname given at launch
 */
TEST_F(ConnectionScreenFixture, FieldsStartWithTheLaunchValues) {
  EXPECT_NE(writingOf(canvas(), "192.168.1.20"), nullptr);
  EXPECT_NE(writingOf(canvas(), "4243"), nullptr);
  EXPECT_NE(writingOf(canvas(), "ACE"), nullptr);
}

/**
 * Given a connection
 * When the connection screen is built over it
 * Then the screen drops any earlier attempt
 */
TEST_F(ConnectionScreenFixture, BuildingTheScreenDropsAnEarlierAttempt) {
  EXPECT_THAT(connection().calls(), ElementsAre(CANCEL));
}

/**
 * Given the connection screen, its address field selected
 * When Enter is pressed and released, and the press lasts
 * Then an attempt starts with the launch values
 */
TEST_F(ConnectionScreenFixture, EnterConnectsWithTheValuesOfTheFields) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(connectAttemptsOf(connection()),
              ElementsAre(ServerConnectionCall{
                  .request = ServerConnectionCall::Request::Connect,
                  .server = {.address = std::string(LAUNCH_ADDRESS),
                             .port = LAUNCH_PORT},
                  .nickname = std::string(LAUNCH_NICKNAME)}));
}

/**
 * Given the connection screen, its port field selected and erased
 * When 5000 is typed, then Enter chosen
 * Then the attempt goes to port 5000
 */
TEST_F(ConnectionScreenFixture, ATypedPortIsTheOneConnectedTo) {
  tap(sf::Keyboard::Key::Down);
  erase(LAUNCH_PORT_TEXT.size());
  type(TYPED_PORT_TEXT);

  choose(sf::Keyboard::Key::Enter);

  const std::vector<ServerConnectionCall> requests =
      connectAttemptsOf(connection());
  ASSERT_EQ(requests.size(), 1U);
  EXPECT_EQ(requests.front().server.port, TYPED_PORT);
}

/**
 * Given the connection screen, its address cut to 192.168.1.
 * When Enter is chosen
 * Then the status line names the address
 */
TEST_F(ConnectionScreenFixture, AnAddressThatIsNotIpv4IsNamed) {
  erase(LAST_ADDRESS_NUMBER_TEXT.size());

  choose(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), INVALID_ADDRESS_STATUS), nullptr);
}

/**
 * Given the connection screen, its address cut to 192.168.1.
 * When Enter is chosen
 * Then no attempt starts
 */
TEST_F(ConnectionScreenFixture, AnAddressThatIsNotIpv4StartsNoAttempt) {
  erase(LAST_ADDRESS_NUMBER_TEXT.size());

  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(connectAttemptsOf(connection()), IsEmpty());
}

/**
 * Given the connection screen, its port replaced by 0
 * When Enter is chosen
 * Then the status line names the port
 */
TEST_F(ConnectionScreenFixture, PortZeroIsNamed) {
  tap(sf::Keyboard::Key::Down);
  erase(LAUNCH_PORT_TEXT.size());
  type(PORT_ZERO_TEXT);

  choose(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), INVALID_PORT_STATUS), nullptr);
}

/**
 * Given the connection screen, its nickname erased
 * When Enter is chosen
 * Then the status line asks for a callsign
 */
TEST_F(ConnectionScreenFixture, AnEmptyNicknameIsNamed) {
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Down);
  erase(LAUNCH_NICKNAME.size());

  choose(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), MISSING_NICKNAME_STATUS), nullptr);
}

/**
 * Given the connection screen, after a refused address was fixed
 * When Enter is chosen again
 * Then the status line no longer names the address
 */
TEST_F(ConnectionScreenFixture, AFixedFieldIsNoLongerNamed) {
  erase(LAST_ADDRESS_NUMBER_TEXT.size());
  choose(sf::Keyboard::Key::Enter);
  type(LAST_ADDRESS_NUMBER_TEXT);

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(writingOf(canvas(), INVALID_ADDRESS_STATUS), nullptr);
}

/**
 * Given an attempt in progress
 * When a frame goes by
 * Then the status line says the client is connecting
 */
TEST_F(ConnectionScreenFixture, ConnectingIsShown) {
  show({.phase = ConnectionPhase::Connecting});

  EXPECT_NE(writingOf(canvas(), CONNECTING_STATUS), nullptr);
}

/**
 * Given an attempt the server accepted as the third player
 * When a frame goes by
 * Then the status line gives that number
 */
TEST_F(ConnectionScreenFixture, AnAcceptedPlayerSeesItsNumber) {
  show({.phase = ConnectionPhase::Accepted, .playerNumber = THIRD_PLAYER});

  EXPECT_NE(writingOf(canvas(), "CONNECTED AS PLAYER 3"), nullptr);
}

/**
 * Given an attempt no answer came to
 * When a frame goes by
 * Then the status line says the server cannot be reached
 */
TEST_F(ConnectionScreenFixture, AnUnreachableServerIsShown) {
  show({.phase = ConnectionPhase::Unreachable});

  EXPECT_NE(writingOf(canvas(), UNREACHABLE_STATUS), nullptr);
}

/**
 * Given a screen whose status line said the server cannot be reached
 * When the connection reports a lost session
 * Then nothing is written where the status line goes
 */
TEST_F(ConnectionScreenFixture, ALostSessionShowsNoStatusLine) {
  show({.phase = ConnectionPhase::Unreachable});
  const RecordedWriting* unreachable = writingOf(canvas(), UNREACHABLE_STATUS);
  ASSERT_NE(unreachable, nullptr);
  const sf::Vector2i statusAnchor = unreachable->anchor;

  show({.phase = ConnectionPhase::Lost});

  EXPECT_THAT(canvas().writings,
              Not(Contains(Field(&RecordedWriting::anchor, statusAnchor))));
}

/**
 * Given a screen whose status line said the server cannot be reached
 * When the connection reports a session the server closed
 * Then nothing is written where the status line goes
 */
TEST_F(ConnectionScreenFixture, AClosedSessionShowsNoStatusLine) {
  show({.phase = ConnectionPhase::Unreachable});
  const RecordedWriting* unreachable = writingOf(canvas(), UNREACHABLE_STATUS);
  ASSERT_NE(unreachable, nullptr);
  const sf::Vector2i statusAnchor = unreachable->anchor;

  show({.phase = ConnectionPhase::Closed});

  EXPECT_THAT(canvas().writings,
              Not(Contains(Field(&RecordedWriting::anchor, statusAnchor))));
}

/**
 * Given an attempt the server has just accepted
 * When frames go by up to one tick before the acceptance showed long enough
 * Then the screen asks for nothing yet
 */
TEST_F(ConnectionScreenFixture, TheWaitingRoomWaitsUntilTheAcceptanceShowed) {
  show(ACCEPTED_AS_FIRST_PLAYER);

  screen().update(ACCEPTED_STATUS_DURATION - ONE_FRAME - ONE_TICK);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given an attempt the server has just accepted
 * When frames go by until the acceptance showed long enough
 * Then the screen opens the waiting room over itself
 */
TEST_F(ConnectionScreenFixture, TheWaitingRoomOpensOnceTheAcceptanceShowed) {
  show(ACCEPTED_AS_FIRST_PLAYER);

  screen().update(ACCEPTED_STATUS_DURATION - ONE_FRAME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Lobby}));
}

/**
 * Given the waiting room already opened after an acceptance
 * When more frames go by
 * Then the waiting room is opened once only
 */
TEST_F(ConnectionScreenFixture, TheWaitingRoomOpensOnce) {
  show(ACCEPTED_AS_FIRST_PLAYER);
  screen().update(ACCEPTED_STATUS_DURATION);

  screen().update(ONE_FRAME);

  EXPECT_EQ(navigator().calls().size(), 1U);
}

/**
 * Given the waiting room opened, then left, which drops the attempt
 * When the player connects again and the server accepts again
 * Then the waiting room opens a second time
 */
TEST_F(ConnectionScreenFixture, AnotherAttemptOpensTheWaitingRoomAgain) {
  show(ACCEPTED_AS_FIRST_PLAYER);
  screen().update(ACCEPTED_STATUS_DURATION);
  connection().cancel();
  screen().update(ONE_FRAME);

  choose(sf::Keyboard::Key::Enter);
  show(ACCEPTED_AS_FIRST_PLAYER);
  screen().update(ACCEPTED_STATUS_DURATION);

  const NavigatorCall openLobby{.request = NavigatorRequest::Push,
                                .screen = ScreenId::Lobby};
  EXPECT_THAT(navigator().calls(), ElementsAre(openLobby, openLobby));
}

/**
 * Given the connection screen
 * When Escape is pressed and released
 * Then the attempt is dropped
 */
TEST_F(ConnectionScreenFixture, EscapeDropsTheAttempt) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(connection().calls(), ElementsAre(CANCEL, CANCEL));
}

/**
 * Given the connection screen
 * When Escape is pressed and released
 * Then the screen asks to be closed
 */
TEST_F(ConnectionScreenFixture, EscapeClosesTheScreen) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given an attempt the server refused, for each reason
 * When a frame goes by
 * Then the status line gives that reason
 */
TEST_P(ConnectionRefusalFixture, ARefusalShowsItsReason) {
  const auto [reason, line] = GetParam();

  show({.phase = ConnectionPhase::Refused, .refusal = reason});

  EXPECT_NE(writingOf(canvas(), line), nullptr);
}

/**
 * Given the connection screen
 * When Escape activates BACK
 * Then the click of BACK plays once
 */
TEST_F(ConnectionScreenFixture, BackPlaysTheClick) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

INSTANTIATE_TEST_SUITE_P(
    EveryReasonButTheVersion, ConnectionRefusalFixture,
    ::testing::Values(std::tuple(RefusalReason::GameFull,
                                 std::string_view(GAME_FULL_STATUS)),
                      std::tuple(RefusalReason::GameInProgress,
                                 std::string_view(GAME_IN_PROGRESS_STATUS)),
                      std::tuple(RefusalReason::NicknameTaken,
                                 std::string_view(NICKNAME_TAKEN_STATUS))));

/**
 * Given an attempt a server of protocol version 2 refused as
 * IncompatibleVersion, the client speaking version 1
 * When a frame goes by
 * Then the status line names both versions, the client's first
 */
TEST_F(ConnectionScreenFixture, AVersionRefusalNamesBothVersions) {
  show({.phase = ConnectionPhase::Refused,
        .refusal = RefusalReason::IncompatibleVersion,
        .serverVersion = NEXT_PROTOCOL_VERSION});

  EXPECT_NE(writingOf(canvas(), "REFUSED: YOU V1, SERVER V2"), nullptr);
}

/**
 * Given a connection screen whose player connected as NOVA
 * When a new connection screen opens and the player chooses Enter at once
 * Then the attempt goes out as NOVA, to the server of the launch values
 */
TEST_F(ConnectionScreenChoiceFixture, ANewScreenStartsWithTheLastAttempt) {
  connectAs(TYPED_NICKNAME_TEXT);

  reopen();
  chooseEnter();

  EXPECT_EQ(
      connectAttemptsOf(connection()).back(),
      (ServerConnectionCall{.request = ServerConnectionCall::Request::Connect,
                            .server = {.address = std::string(LAUNCH_ADDRESS),
                                       .port = LAUNCH_PORT},
                            .nickname = std::string(TYPED_NICKNAME)}));
}

/**
 * Given a connection screen whose nickname field was emptied, then Enter
 * chosen, which the empty nickname refuses
 * When a new connection screen opens and Enter is chosen
 * Then the attempt goes out with the launch nickname: a refused attempt
 * leaves the choice
 */
TEST_F(ConnectionScreenChoiceFixture, AnAttemptTheFieldsRefuseKeepsTheChoice) {
  connectAs(NO_NICKNAME_TEXT);

  reopen();
  chooseEnter();

  EXPECT_EQ(connectAttemptsOf(connection()).back().nickname, LAUNCH_NICKNAME);
}
