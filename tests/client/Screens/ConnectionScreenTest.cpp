#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include "ConnectionRefused.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenConstants.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "ScreenId.hpp"
#include "ScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ACCEPTED_STATUS_DURATION;
using rtype::client::CONNECTING_STATUS;
using rtype::client::ConnectionScreen;
using rtype::client::ConnectionStatus;
using rtype::client::GAME_FULL_STATUS;
using rtype::client::GAME_IN_PROGRESS_STATUS;
using rtype::client::INCOMPATIBLE_VERSION_STATUS;
using rtype::client::INVALID_ADDRESS_STATUS;
using rtype::client::INVALID_PORT_STATUS;
using rtype::client::MISSING_NICKNAME_STATUS;
using rtype::client::PRESS_DURATION;
using rtype::client::ScreenId;
using rtype::client::UNREACHABLE_STATUS;
using rtype::network::Endpoint;
using rtype::network::RefusalReason;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

/** @brief What the launch options gave: the fields start with them. */
constexpr std::string_view LAUNCH_ADDRESS = "192.168.1.20";
constexpr std::uint16_t LAUNCH_PORT = 4243;
constexpr std::string_view LAUNCH_NICKNAME = "ACE";

/** @brief A port the player types, as typed and as read. */
constexpr std::u32string_view TYPED_PORT_TEXT = U"5000";
constexpr std::uint16_t TYPED_PORT = 5000;

/** @brief A port one below the lowest port. */
constexpr std::u32string_view PORT_ZERO_TEXT = U"0";

/** @brief The last number of the launch address, typed again. */
constexpr std::u32string_view LAST_ADDRESS_NUMBER_TEXT = U"20";

constexpr std::uint8_t THIRD_PLAYER = 3;
constexpr std::uint8_t FIRST_PLAYER = 1;

/** @brief The cancel the screen sends when it is built, before any request of
 * a test. */
const ServerConnectionCall CANCEL{
    .request = ServerConnectionCall::Request::Cancel,
    .server = {},
    .nickname = {}};

/** @brief A connection screen alone, with a recording navigator, connection
 * and canvas, its fields started with the launch values. */
class ConnectionScreenTest : public ::testing::Test {
 protected:
  [[nodiscard]] const RecordingNavigator& navigator() const {
    return navigator_;
  }

  RecordingServerConnection& connection() { return connection_; }

  [[nodiscard]] const InterfaceCanvasRecord& canvas() const { return canvas_; }

  ConnectionScreen& screen() { return screen_; }

  void tap(sf::Keyboard::Key key) {
    screen_.handleEvent(keyPress(key));
    screen_.handleEvent(keyRelease(key));
  }

  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key) {
    tap(key);
    screen_.update(PRESS_DURATION);
  }

  void type(std::u32string_view text) {
    for (const char32_t character : text) {
      screen_.handleEvent(typedCharacter(character));
    }
  }

  /** @brief Erases characters from the selected field, Backspace after
   * Backspace. */
  void erase(std::size_t characters) {
    for (std::size_t erased = 0; erased < characters; ++erased) {
      screen_.handleEvent(keyPress(sf::Keyboard::Key::Backspace));
    }
  }

  /** @brief Selects the port field and replaces the launch port with a text.
   */
  void replacePort(std::u32string_view text) {
    tap(sf::Keyboard::Key::Down);
    erase(std::to_string(LAUNCH_PORT).size());
    type(text);
  }

  /** @brief Gives the connection a status, then lets one frame go by. */
  void show(ConnectionStatus status) {
    connection_.setStatus(status);
    screen_.update(ONE_FRAME);
  }

  /** @brief Shows that the server has just accepted the first player. */
  void showAccepted() {
    show({.phase = ConnectionStatus::Phase::Accepted,
          .playerNumber = FIRST_PLAYER});
  }

  /** @returns The requests to start an attempt, without the cancels. */
  [[nodiscard]] std::vector<ServerConnectionCall> attempts() const {
    std::vector<ServerConnectionCall> connects;
    for (const ServerConnectionCall& call : connection_.calls()) {
      if (call.request == ServerConnectionCall::Request::Connect) {
        connects.push_back(call);
      }
    }
    return connects;
  }

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  ConnectionScreen screen_{
      navigator_,
      connection_,
      std::make_unique<RecordingInterfaceCanvas>(canvas_),
      samePlayfieldPoint,
      Endpoint{.address = std::string(LAUNCH_ADDRESS), .port = LAUNCH_PORT},
      LAUNCH_NICKNAME};
};

/** @brief A connection screen whose attempt the server refused, for one
 * reason, and the line it must show. */
class RefusalTest : public ConnectionScreenTest,
                    public ::testing::WithParamInterface<
                        std::tuple<RefusalReason, std::string_view>> {};

}  // namespace

/**
 * Given the connection screen just built with the launch values
 * When it is painted
 * Then its fields read the address, the port and the nickname given at launch
 */
TEST_F(ConnectionScreenTest, FieldsStartWithTheLaunchValues) {
  EXPECT_NE(writingOf(canvas(), "192.168.1.20"), nullptr);
  EXPECT_NE(writingOf(canvas(), "4243"), nullptr);
  EXPECT_NE(writingOf(canvas(), "ACE"), nullptr);
}

/**
 * Given a connection
 * When the connection screen is built over it
 * Then the screen drops any earlier attempt
 */
TEST_F(ConnectionScreenTest, BuildingTheScreenDropsAnEarlierAttempt) {
  EXPECT_THAT(connection().calls(), ElementsAre(CANCEL));
}

/**
 * Given the connection screen, its address field selected
 * When Enter is pressed and released, and the press lasts
 * Then an attempt starts with the launch values
 */
TEST_F(ConnectionScreenTest, EnterConnectsWithTheValuesOfTheFields) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(attempts(), ElementsAre(ServerConnectionCall{
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
TEST_F(ConnectionScreenTest, ATypedPortIsTheOneConnectedTo) {
  replacePort(TYPED_PORT_TEXT);

  choose(sf::Keyboard::Key::Enter);

  const std::vector<ServerConnectionCall> requests = attempts();
  ASSERT_EQ(requests.size(), 1U);
  EXPECT_EQ(requests.front().server.port, TYPED_PORT);
}

/**
 * Given the connection screen, its address cut to 192.168.1.
 * When Enter is chosen
 * Then the status line names the address
 */
TEST_F(ConnectionScreenTest, AnAddressThatIsNotIpv4IsNamed) {
  erase(LAST_ADDRESS_NUMBER_TEXT.size());

  choose(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), INVALID_ADDRESS_STATUS), nullptr);
}

/**
 * Given the connection screen, its address cut to 192.168.1.
 * When Enter is chosen
 * Then no attempt starts
 */
TEST_F(ConnectionScreenTest, AnAddressThatIsNotIpv4StartsNoAttempt) {
  erase(LAST_ADDRESS_NUMBER_TEXT.size());

  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(attempts(), IsEmpty());
}

/**
 * Given the connection screen, its port replaced by 0
 * When Enter is chosen
 * Then the status line names the port
 */
TEST_F(ConnectionScreenTest, PortZeroIsNamed) {
  replacePort(PORT_ZERO_TEXT);

  choose(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), INVALID_PORT_STATUS), nullptr);
}

/**
 * Given the connection screen, its nickname erased
 * When Enter is chosen
 * Then the status line asks for a callsign
 */
TEST_F(ConnectionScreenTest, AnEmptyNicknameIsNamed) {
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
TEST_F(ConnectionScreenTest, AFixedFieldIsNoLongerNamed) {
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
TEST_F(ConnectionScreenTest, ConnectingIsShown) {
  show({.phase = ConnectionStatus::Phase::Connecting});

  EXPECT_NE(writingOf(canvas(), CONNECTING_STATUS), nullptr);
}

/**
 * Given an attempt the server accepted as the third player
 * When a frame goes by
 * Then the status line gives that number
 */
TEST_F(ConnectionScreenTest, AnAcceptedPlayerSeesItsNumber) {
  show({.phase = ConnectionStatus::Phase::Accepted,
        .playerNumber = THIRD_PLAYER});

  EXPECT_NE(writingOf(canvas(), "CONNECTED AS PLAYER 3"), nullptr);
}

/**
 * Given an attempt no answer came to
 * When a frame goes by
 * Then the status line says the server cannot be reached
 */
TEST_F(ConnectionScreenTest, AnUnreachableServerIsShown) {
  show({.phase = ConnectionStatus::Phase::Unreachable});

  EXPECT_NE(writingOf(canvas(), UNREACHABLE_STATUS), nullptr);
}

/**
 * Given an attempt the server has just accepted
 * When frames go by up to one tick before the acceptance showed long enough
 * Then the screen asks for nothing yet
 */
TEST_F(ConnectionScreenTest, TheGameWaitsUntilTheAcceptanceShowed) {
  showAccepted();

  screen().update(ACCEPTED_STATUS_DURATION - ONE_FRAME - ONE_TICK);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given an attempt the server has just accepted
 * When frames go by until the acceptance showed long enough
 * Then the screen asks for the game in place of every screen
 */
TEST_F(ConnectionScreenTest, TheGameIsAskedOnceTheAcceptanceShowed) {
  showAccepted();

  screen().update(ACCEPTED_STATUS_DURATION - ONE_FRAME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Game}));
}

/**
 * Given the game already asked for after an acceptance
 * When more frames go by
 * Then the game is asked for once only
 */
TEST_F(ConnectionScreenTest, TheGameIsAskedOnce) {
  showAccepted();
  screen().update(ACCEPTED_STATUS_DURATION);

  screen().update(ONE_FRAME);

  EXPECT_EQ(navigator().calls().size(), 1U);
}

/**
 * Given the connection screen
 * When Escape is pressed and released
 * Then the attempt is dropped
 */
TEST_F(ConnectionScreenTest, EscapeDropsTheAttempt) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(connection().calls(), ElementsAre(CANCEL, CANCEL));
}

/**
 * Given the connection screen
 * When Escape is pressed and released
 * Then the screen asks to be closed
 */
TEST_F(ConnectionScreenTest, EscapeClosesTheScreen) {
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
TEST_P(RefusalTest, ARefusalShowsItsReason) {
  const auto [reason, line] = GetParam();

  show({.phase = ConnectionStatus::Phase::Refused, .refusal = reason});

  EXPECT_NE(writingOf(canvas(), line), nullptr);
}

INSTANTIATE_TEST_SUITE_P(
    EveryReason, RefusalTest,
    ::testing::Values(
        std::tuple(RefusalReason::GameFull, std::string_view(GAME_FULL_STATUS)),
        std::tuple(RefusalReason::GameInProgress,
                   std::string_view(GAME_IN_PROGRESS_STATUS)),
        std::tuple(RefusalReason::IncompatibleVersion,
                   std::string_view(INCOMPATIBLE_VERSION_STATUS))));
