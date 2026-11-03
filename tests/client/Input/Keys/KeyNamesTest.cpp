#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <set>
#include <string_view>
#include "InputConstants.hpp"
#include "KeyNames.hpp"

using rtype::client::KEY_NAMES;
using rtype::client::keyName;
using rtype::client::keyNamed;
using rtype::client::NamedKey;
using Scan = sf::Keyboard::Scan;

/**
 * Given every key the player may bind
 * When the name it is given is read back
 * Then it gives that key again
 */
TEST(KeyNamesTest, EveryNameGivesBackItsKey) {
  for (const NamedKey& named : KEY_NAMES) {
    EXPECT_EQ(keyNamed(named.name), std::optional(named.key)) << named.name;
  }
}

/**
 * Given every key the player may bind
 * When their names are gathered
 * Then no two keys share a name, so a file names one key per line
 */
TEST(KeyNamesTest, NoTwoKeysShareAName) {
  std::set<std::string_view> names;
  for (const NamedKey& named : KEY_NAMES) {
    EXPECT_TRUE(names.insert(named.name).second) << named.name;
  }
}

/**
 * Given every key the player may bind
 * When each is named
 * Then it gets the name it is listed with, so no key is listed twice
 */
TEST(KeyNamesTest, EveryKeyGetsItsListedName) {
  for (const NamedKey& named : KEY_NAMES) {
    EXPECT_EQ(keyName(named.key), std::optional(named.name)) << named.name;
  }
}

/**
 * Given Escape, the key that opens and closes the menus
 * When it is named
 * Then it has no name, so it can never be bound to an action
 */
TEST(KeyNamesTest, EscapeCannotBeBound) {
  EXPECT_FALSE(keyName(Scan::Escape).has_value());
}

/**
 * Given the Command keys of a Mac, which are the Windows keys of a PC
 * When they are named
 * Then they have no name, so their system shortcuts never fire in game
 */
TEST(KeyNamesTest, TheCommandAndWindowsKeysCannotBeBound) {
  EXPECT_FALSE(keyName(Scan::LSystem).has_value());
  EXPECT_FALSE(keyName(Scan::RSystem).has_value());
}

/**
 * Given the keys of an Apple extended keyboard: Option, the equals sign of
 * its keypad, F13 to F19
 * When they are named
 * Then each has a name, so a Mac player may bind them
 */
TEST(KeyNamesTest, TheKeysOfAMacKeyboardCanBeBound) {
  EXPECT_EQ(keyName(Scan::LAlt), std::optional<std::string_view>("LEFT ALT"));
  EXPECT_EQ(keyName(Scan::NumpadEqual),
            std::optional<std::string_view>("PAD ="));
  EXPECT_EQ(keyName(Scan::F19), std::optional<std::string_view>("F19"));
}

/**
 * Given texts that no key is listed with, a lowercase name among them
 * When they are read as names
 * Then they name no key
 */
TEST(KeyNamesTest, AnUnlistedTextNamesNoKey) {
  EXPECT_FALSE(keyNamed("SPACEBAR").has_value());
  EXPECT_FALSE(keyNamed("space").has_value());
  EXPECT_FALSE(keyNamed("").has_value());
}
