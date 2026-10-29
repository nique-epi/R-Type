#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include "EventListeners.hpp"
#include "EventListenersFixtures.hpp"
#include "EventListenersTestConstants.hpp"
#include "EventUse.hpp"
#include "KeyPress.hpp"
#include "SubscriptionHandle.hpp"

using rtype::client::EventListeners;
using rtype::client::EventUse;
using rtype::engine::SubscriptionHandle;

/**
 * Given a listener that records the keys pressed
 * When a key is pressed
 * Then the listener is shown that key
 */
TEST(EventListeners, ShowsTheOfferedEventToTheListener) {
  EventListeners listeners;
  std::vector<sf::Keyboard::Key> pressedKeys;
  static_cast<void>(listeners.add(keyRecordingListener(pressedKeys)));

  listeners.offer(keyPress(GLOBAL_KEY));

  EXPECT_EQ(pressedKeys, (std::vector<sf::Keyboard::Key>{GLOBAL_KEY}));
}

/**
 * Given the listener of the screens, then a listener of a global key that
 * passes every event on
 * When a key is pressed
 * Then the global key listener sees it first, then the screens
 */
TEST(EventListeners, ShowsAnEventToTheLastAddedListenerFirst) {
  EventListeners listeners;
  std::vector<int> seen;
  static_cast<void>(
      listeners.add(recordingListener(seen, SCREENS_LISTENER, EventUse::Kept)));
  static_cast<void>(listeners.add(
      recordingListener(seen, GLOBAL_KEY_LISTENER, EventUse::Passed)));

  listeners.offer(keyPress(GLOBAL_KEY));

  EXPECT_EQ(seen, (std::vector<int>{GLOBAL_KEY_LISTENER, SCREENS_LISTENER}));
}

/**
 * Given the listener of the screens, then a listener that keeps its global
 * key
 * When that key is pressed
 * Then the screens never see it
 */
TEST(EventListeners, StopsAnEventAtTheListenerThatKeepsIt) {
  EventListeners listeners;
  std::vector<int> seen;
  static_cast<void>(
      listeners.add(recordingListener(seen, SCREENS_LISTENER, EventUse::Kept)));
  static_cast<void>(listeners.add(
      recordingListener(seen, GLOBAL_KEY_LISTENER, EventUse::Kept)));

  listeners.offer(keyPress(GLOBAL_KEY));

  EXPECT_EQ(seen, (std::vector<int>{GLOBAL_KEY_LISTENER}));
}

/**
 * Given the listener of the screens, then a listener of a global key that
 * was removed
 * When a key is pressed
 * Then only the screens see it
 */
TEST(EventListeners, ShowsNothingToARemovedListener) {
  EventListeners listeners;
  std::vector<int> seen;
  static_cast<void>(
      listeners.add(recordingListener(seen, SCREENS_LISTENER, EventUse::Kept)));
  const SubscriptionHandle global = listeners.add(
      recordingListener(seen, GLOBAL_KEY_LISTENER, EventUse::Kept));
  listeners.remove(global);

  listeners.offer(keyPress(GLOBAL_KEY));

  EXPECT_EQ(seen, (std::vector<int>{SCREENS_LISTENER}));
}

/**
 * Given a listener added under a handle
 * When it is removed
 * Then the removal reports that a listener had that handle
 */
TEST(EventListeners, ReportsTheRemovalOfAnAddedListener) {
  EventListeners listeners;
  std::vector<int> seen;
  const SubscriptionHandle handle =
      listeners.add(recordingListener(seen, SCREENS_LISTENER, EventUse::Kept));

  const bool removed = listeners.remove(handle);

  EXPECT_TRUE(removed);
}

/**
 * Given a listener removed once
 * When it is removed again
 * Then the second removal reports that no listener had that handle
 */
TEST(EventListeners, ReportsARemovalOfAListenerAlreadyRemoved) {
  EventListeners listeners;
  std::vector<int> seen;
  const SubscriptionHandle handle =
      listeners.add(recordingListener(seen, SCREENS_LISTENER, EventUse::Kept));
  listeners.remove(handle);

  const bool removed = listeners.remove(handle);

  EXPECT_FALSE(removed);
}
