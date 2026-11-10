#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "RecordingNavigator.hpp"
#include "UpdateScreenTestConstants.hpp"
#include "WhatsNewScreenFixture.hpp"

using rtype::client::testing::FIRST_NOTE;
using rtype::client::testing::SECOND_NOTE;
using rtype::client::testing::WHATS_NEW_HEADING_OF_THE_NOTES;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given WHAT'S NEW built with the notes of 1.1.0
 * When it is shown
 * Then it writes the version and every note
 */
TEST_F(WhatsNewScreenFixture, ShowsTheVersionAndEveryNote) {
  EXPECT_TRUE(wrote(WHATS_NEW_HEADING_OF_THE_NOTES));
  EXPECT_TRUE(wrote(FIRST_NOTE));
  EXPECT_TRUE(wrote(SECOND_NOTE));
}

/**
 * Given WHAT'S NEW shown
 * When Enter is pressed
 * Then it closes and marks the notes read
 */
TEST_F(WhatsNewScreenFixture, EnterClosesItAndMarksTheNotesRead) {
  screen().handleEvent(sf::Event::KeyPressed{.code = sf::Keyboard::Key::Enter});

  EXPECT_EQ(closes(), 1);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given WHAT'S NEW shown
 * When Enter is pressed twice with no release between
 * Then it closes once
 */
TEST_F(WhatsNewScreenFixture, AHeldKeyRepeatedClosesItOnce) {
  screen().handleEvent(sf::Event::KeyPressed{.code = sf::Keyboard::Key::Enter});
  screen().handleEvent(sf::Event::KeyPressed{.code = sf::Keyboard::Key::Enter});

  EXPECT_EQ(closes(), 1);
}

/**
 * Given WHAT'S NEW shown
 * When a click and an Escape arrive in the same frame
 * Then it asks to close once
 */
TEST_F(WhatsNewScreenFixture, TwoWaysOfClosingInOneFrameCloseItOnce) {
  screen().handleEvent(sf::Event::MouseButtonPressed{});
  screen().handleEvent(
      sf::Event::KeyPressed{.code = sf::Keyboard::Key::Escape});

  EXPECT_EQ(closes(), 1);
  EXPECT_EQ(navigator().calls().size(), 1U);
}

/**
 * Given WHAT'S NEW shown
 * When A is pressed
 * Then it stays
 */
TEST_F(WhatsNewScreenFixture, AnotherKeyDoesNotCloseIt) {
  screen().handleEvent(sf::Event::KeyPressed{.code = sf::Keyboard::Key::A});

  EXPECT_EQ(closes(), 0);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}
