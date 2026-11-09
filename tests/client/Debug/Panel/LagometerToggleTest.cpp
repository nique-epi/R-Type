#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "EventUse.hpp"
#include "KeyPress.hpp"
#include "LagometerConstants.hpp"
#include "LagometerToggle.hpp"

namespace rtype::client {

/**
 * Given a toggle
 * When F3 is pressed
 * Then the lagometer shows, and no screen hears the key
 */
TEST(LagometerToggle, APressOfTheKeyShowsTheLagometer) {
  LagometerToggle toggle;

  const EventUse use =
      toggle.handle(keyPress(sf::Keyboard::Key::F3, LAGOMETER_KEY));

  EXPECT_TRUE(toggle.shown());
  EXPECT_EQ(use, EventUse::Kept);
}

/**
 * Given a toggle that F3 showed, still held
 * When the press of F3 repeats
 * Then the lagometer stays shown
 */
TEST(LagometerToggle, AHeldKeyTogglesOnce) {
  LagometerToggle toggle;
  toggle.handle(keyPress(sf::Keyboard::Key::F3, LAGOMETER_KEY));

  toggle.handle(keyPress(sf::Keyboard::Key::F3, LAGOMETER_KEY));

  EXPECT_TRUE(toggle.shown());
}

/**
 * Given a toggle that F3 showed
 * When F3 is released, then pressed again
 * Then the lagometer hides
 */
TEST(LagometerToggle, APressAfterTheReleaseHidesTheLagometer) {
  LagometerToggle toggle;
  toggle.handle(keyPress(sf::Keyboard::Key::F3, LAGOMETER_KEY));
  toggle.handle(keyRelease(sf::Keyboard::Key::F3, LAGOMETER_KEY));

  toggle.handle(keyPress(sf::Keyboard::Key::F3, LAGOMETER_KEY));

  EXPECT_FALSE(toggle.shown());
}

/**
 * Given a toggle that F3 showed, still held when the window lost the focus
 * When F3 is pressed once the window has it back
 * Then the lagometer hides: the release lost with the focus waits no more
 */
TEST(LagometerToggle, ALostFocusEndsTheHold) {
  LagometerToggle toggle;
  toggle.handle(keyPress(sf::Keyboard::Key::F3, LAGOMETER_KEY));
  toggle.handle(sf::Event{sf::Event::FocusLost{}});

  toggle.handle(keyPress(sf::Keyboard::Key::F3, LAGOMETER_KEY));

  EXPECT_FALSE(toggle.shown());
}

/**
 * Given a toggle
 * When another key is pressed
 * Then it goes on to the screens, and the lagometer stays hidden
 */
TEST(LagometerToggle, AnotherKeyGoesOnToTheScreens) {
  LagometerToggle toggle;

  const EventUse use = toggle.handle(
      keyPress(sf::Keyboard::Key::Space, sf::Keyboard::Scan::Space));

  EXPECT_EQ(use, EventUse::Passed);
  EXPECT_FALSE(toggle.shown());
}

}  // namespace rtype::client
