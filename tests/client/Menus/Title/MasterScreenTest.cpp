#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include "ConnectionScreenConstants.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "Endpoint.hpp"
#include "MasterScreenFixture.hpp"
#include "MasterScreenTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::INVALID_ADDRESS_STATUS;
using rtype::client::INVALID_PORT_STATUS;
using rtype::client::testing::FORM_MASTER_ADDRESS;
using rtype::client::testing::FORM_MASTER_PORT;
using rtype::client::testing::FORM_MASTER_PORT_TEXT;
using rtype::client::testing::PORT_ZERO_TEXT;
using rtype::client::testing::TYPED_MASTER_PORT;
using rtype::client::testing::TYPED_MASTER_PORT_TEXT;
using rtype::network::Endpoint;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;

/**
 * Given the form opened on the master at 10.0.0.2:8080
 * When Enter is chosen on its untouched fields
 * Then the player's master is that master
 */
TEST_F(MasterScreenFixture, TheFieldsStartWithTheMasterRead) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(settings().master,
            std::optional(Endpoint{.address = std::string{FORM_MASTER_ADDRESS},
                                   .port = FORM_MASTER_PORT}));
}

/**
 * Given the form, its port replaced by 9090
 * When Enter is chosen
 * Then the player's master listens on 9090
 */
TEST_F(MasterScreenFixture, SaveKeepsTheTypedMaster) {
  tap(sf::Keyboard::Key::Down);
  erase(FORM_MASTER_PORT_TEXT.size());
  type(TYPED_MASTER_PORT_TEXT);

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(settings().master,
            std::optional(Endpoint{.address = std::string{FORM_MASTER_ADDRESS},
                                   .port = TYPED_MASTER_PORT}));
}

/**
 * Given the form
 * When Enter is chosen
 * Then the settings are written once, naming the master
 */
TEST_F(MasterScreenFixture, SaveWritesTheSettings) {
  choose(sf::Keyboard::Key::Enter);

  ASSERT_THAT(saves(), SizeIs(1));
  EXPECT_EQ(saves().front().master, settings().master);
}

/**
 * Given the form
 * When Enter is chosen
 * Then the form asks to be closed
 */
TEST_F(MasterScreenFixture, SaveClosesTheForm) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the form, its port replaced by 0
 * When Enter is chosen
 * Then the status line names the port
 */
TEST_F(MasterScreenFixture, AnInvalidPortIsNamed) {
  tap(sf::Keyboard::Key::Down);
  erase(FORM_MASTER_PORT_TEXT.size());
  type(PORT_ZERO_TEXT);

  choose(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), INVALID_PORT_STATUS), nullptr);
}

/**
 * Given the form, its port replaced by 0
 * When Enter is chosen
 * Then nothing is kept, nothing is saved and the form stays open
 */
TEST_F(MasterScreenFixture, AnInvalidPortKeepsNothing) {
  tap(sf::Keyboard::Key::Down);
  erase(FORM_MASTER_PORT_TEXT.size());
  type(PORT_ZERO_TEXT);

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(settings().master, std::nullopt);
  EXPECT_THAT(saves(), IsEmpty());
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the form opened on a master named by host
 * When Enter is chosen on its untouched fields
 * Then the status line names the address, which starts empty
 */
TEST_F(MasterScreenForAHostFixture, AMasterNamedByHostStartsWithNoAddress) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), INVALID_ADDRESS_STATUS), nullptr);
}

/**
 * Given the form
 * When Escape is pressed and released
 * Then the form asks to be closed
 */
TEST_F(MasterScreenFixture, EscapeClosesTheForm) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the form
 * When Escape is pressed and released
 * Then the player has chosen no master and nothing is saved
 */
TEST_F(MasterScreenFixture, BackLeavesTheMasterAsItWas) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_EQ(settings().master, std::nullopt);
  EXPECT_THAT(saves(), IsEmpty());
}
