#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardFilterDialogFixture.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::LeaderboardFilterChoice;

/**
 * Given the dialog open
 * When it is painted
 * Then it writes its rows
 */
TEST_F(LeaderboardFilterDialogFixture, ItWritesItsRows) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  dialog().paint(canvas);

  EXPECT_NE(writingOf(record, "DIFFICULTY"), nullptr);
  EXPECT_NE(writingOf(record, "PILOTS"), nullptr);
}

/**
 * Given the dialog open
 * When the player steps both rows, then reaches APPLY and presses it
 * Then what the player picked comes back, and the dialog closes
 */
TEST_F(LeaderboardFilterDialogFixture, ApplyGivesBackThePick) {
  choose(sf::Keyboard::Key::Right);
  choose(sf::Keyboard::Key::Down);
  choose(sf::Keyboard::Key::Right);
  choose(sf::Keyboard::Key::Down);
  choose(sf::Keyboard::Key::Down);
  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(applied(), std::optional(LeaderboardFilterChoice{.difficulty = 1,
                                                             .pilots = 1}));
  EXPECT_FALSE(dialog().isOpen());
}

/**
 * Given the dialog open and a value stepped
 * When Escape is pressed and released
 * Then nothing comes back, and the dialog closes
 */
TEST_F(LeaderboardFilterDialogFixture, EscapeChangesNothing) {
  choose(sf::Keyboard::Key::Right);

  choose(sf::Keyboard::Key::Escape);

  EXPECT_EQ(applied(), std::nullopt);
  EXPECT_FALSE(dialog().isOpen());
}
