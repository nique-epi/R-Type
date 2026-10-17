#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include "BarState.hpp"
#include "KeyPress.hpp"
#include "KeySlot.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::BarState;
using rtype::client::KeySlot;
using Key = sf::Keyboard::Key;
using Scan = sf::Keyboard::Scan;

namespace {

/** @brief The height of every control of a row. */
constexpr int CONTROL_HEIGHT_ON_THE_PICTURE = 12;

/** @brief A key the slot refuses, as a key without a name would be. */
constexpr sf::Keyboard::Scancode REFUSED = Scan::LSystem;

class KeySlotTest : public ::testing::Test {
 protected:
  [[nodiscard]] sf::Keyboard::Scancode key() const { return key_; }
  [[nodiscard]] int writes() const { return writes_; }
  void share() { shared_ = true; }
  KeySlot& slot() { return slot_; }

  [[nodiscard]] bool paintingWrites(const std::string& text) const {
    InterfaceCanvasRecord record;
    RecordingInterfaceCanvas canvas(record);
    slot_.paint(canvas,
                {{0, 0}, {slot_.width(), CONTROL_HEIGHT_ON_THE_PICTURE}},
                BarState::Idle);
    return writingOf(record, text) != nullptr;
  }

 private:
  sf::Keyboard::Scancode key_ = Scan::Space;
  int writes_ = 0;
  bool shared_ = false;
  KeySlot slot_{KeySlot::Wiring{
      .key = {.read = [this] { return key_; },
              .write =
                  [this](sf::Keyboard::Scancode key) {
                    key_ = key;
                    ++writes_;
                  }},
      .name = [](sf::Keyboard::Scancode) { return std::string("KEY"); },
      .accepts = [](sf::Keyboard::Scancode key) { return key != REFUSED; },
      .conflicts = [this] { return shared_; }}};
};

}  // namespace

/**
 * Given the slot of shoot, on space
 * When its row is activated, as Enter does
 * Then it waits for a key and holds the input
 */
TEST_F(KeySlotTest, ActivatingItWaitsForAKey) {
  slot().activate();
  EXPECT_TRUE(slot().isListening());
  EXPECT_TRUE(slot().holdsInput());
}

/**
 * Given the slot waiting
 * When W is pressed
 * Then W is the key of the action, and the wait is over
 */
TEST_F(KeySlotTest, TheNextKeyBecomesItsKey) {
  slot().activate();
  slot().keyPressed(keyPressOf(Key::W, Scan::W));
  EXPECT_EQ(key(), Scan::W);
  EXPECT_FALSE(slot().isListening());
}

/**
 * Given W just taken, still held
 * When the system repeats its press, then W is released
 * Then the repeat is swallowed, and the input is let go on the release
 */
TEST_F(KeySlotTest, RepeatsOfTheKeyTakenAreSwallowedUntilItsRelease) {
  slot().activate();
  slot().keyPressed(keyPressOf(Key::W, Scan::W));
  EXPECT_TRUE(slot().keyPressed(keyPressOf(Key::W, Scan::W)));
  EXPECT_TRUE(slot().holdsInput());
  slot().keyReleased(
      sf::Event::KeyReleased{.code = Key::W, .scancode = Scan::W});
  EXPECT_FALSE(slot().holdsInput());
}

/**
 * Given the slot waiting
 * When Escape is pressed
 * Then the wait is over and the key is kept, without a write
 */
TEST_F(KeySlotTest, EscapeKeepsTheKey) {
  slot().activate();
  EXPECT_TRUE(slot().keyPressed(keyPressOf(Key::Escape, Scan::Escape)));
  EXPECT_FALSE(slot().isListening());
  EXPECT_EQ(key(), Scan::Space);
  EXPECT_EQ(writes(), 0);
}

/**
 * Given the slot waiting
 * When a key it refuses is pressed
 * Then it goes on waiting, the key is kept, and it says the key was refused
 */
TEST_F(KeySlotTest, ARefusedKeyIsIgnored) {
  slot().activate();
  slot().keyPressed(keyPressOf(Key::LSystem, REFUSED));
  EXPECT_TRUE(slot().isListening());
  EXPECT_EQ(key(), Scan::Space);
  EXPECT_TRUE(slot().lastKeyRefused());
}

/**
 * Given the slot waiting
 * When the window loses the focus
 * Then the wait is over
 */
TEST_F(KeySlotTest, LosingTheInputEndsTheWait) {
  slot().activate();
  slot().cancelInput();
  EXPECT_FALSE(slot().holdsInput());
}

/**
 * Given the slot waiting
 * When it is painted
 * Then it says PRESS A KEY
 */
TEST_F(KeySlotTest, WaitingWritesPressAKey) {
  slot().activate();
  EXPECT_TRUE(paintingWrites("PRESS A KEY"));
}

/**
 * Given another action with the same key
 * When the slot is painted
 * Then a flag is written before its keycap
 */
TEST_F(KeySlotTest, ASharedKeyIsFlagged) {
  share();
  EXPECT_TRUE(paintingWrites("!"));
}
