#include <gtest/gtest.h>
#include <string>
#include "ConfigurationRegistryFixture.hpp"
#include "ConfigurationRegistryTestConstants.hpp"
#include "EngineException.hpp"
#include "IntegerSettingDefinition.hpp"

using rtype::engine::DuplicateSettingException;
using rtype::engine::IntegerSettingDefinition;

/**
 * Given a port setting declared with a default
 * When nothing was set
 * Then the value is the default
 */
TEST_F(ConfigurationRegistryFixture, ValueIsTheDefaultBeforeAnySet) {
  EXPECT_EQ(port_->integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given a port setting
 * When `set port` gives a value in bounds
 * Then the line is accepted and the setting holds the value
 */
TEST_F(ConfigurationRegistryFixture, SetInBoundsKeepsTheValue) {
  EXPECT_TRUE(set(PORT_SETTING, PORT_TEXT_IN_BOUNDS).accepted);
  EXPECT_EQ(port_->integer(), PORT_VALUE_IN_BOUNDS);
}

/**
 * Given a port setting
 * When `set port` gives the lowest value
 * Then the value is accepted
 */
TEST_F(ConfigurationRegistryFixture, LowestValueIsAccepted) {
  EXPECT_TRUE(set(PORT_SETTING, PORT_TEXT_LOWEST).accepted);
  EXPECT_EQ(port_->integer(), LOWEST_PORT_VALUE);
}

/**
 * Given a port setting
 * When `set port` gives the highest value
 * Then the value is accepted
 */
TEST_F(ConfigurationRegistryFixture, HighestValueIsAccepted) {
  EXPECT_TRUE(set(PORT_SETTING, PORT_TEXT_HIGHEST).accepted);
  EXPECT_EQ(port_->integer(), HIGHEST_PORT_VALUE);
}

/**
 * Given a port setting
 * When `set port` gives one below the lowest value
 * Then the line is refused with a message naming the bounds, and the old value
 * stays
 */
TEST_F(ConfigurationRegistryFixture, OneBelowTheLowestIsRefused) {
  const auto result = set(PORT_SETTING, PORT_TEXT_BELOW_LOWEST);
  EXPECT_FALSE(result.accepted);
  EXPECT_NE(result.message.find(std::string(PORT_BOUNDS_TEXT)),
            std::string::npos);
  EXPECT_EQ(port_->integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given a port setting
 * When `set port` gives one above the highest value
 * Then the line is refused and the old value stays
 */
TEST_F(ConfigurationRegistryFixture, OneAboveTheHighestIsRefused) {
  EXPECT_FALSE(set(PORT_SETTING, PORT_TEXT_ABOVE_HIGHEST).accepted);
  EXPECT_EQ(port_->integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given a port setting
 * When `set port` gives a word that is no number
 * Then the line is refused and the old value stays
 */
TEST_F(ConfigurationRegistryFixture, NonNumericValueIsRefused) {
  EXPECT_FALSE(set(PORT_SETTING, NOT_A_NUMBER).accepted);
  EXPECT_EQ(port_->integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given a port setting
 * When `set port` gives a number too large for any setting
 * Then the line is refused and the old value stays
 */
TEST_F(ConfigurationRegistryFixture, NumberTooLargeIsRefused) {
  EXPECT_FALSE(set(PORT_SETTING, NUMBER_TOO_LARGE).accepted);
  EXPECT_EQ(port_->integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given the registry
 * When `set` names a setting nobody declared
 * Then the line is refused and the message lists the declared settings
 */
TEST_F(ConfigurationRegistryFixture,
       UndeclaredSettingIsRefusedListingTheKnown) {
  const auto result = set(UNDECLARED_SETTING, PORT_TEXT_IN_BOUNDS);
  EXPECT_FALSE(result.accepted);
  EXPECT_NE(result.message.find(std::string(PORT_SETTING)), std::string::npos);
  EXPECT_NE(result.message.find(std::string(NICKNAME_SETTING)),
            std::string::npos);
}

/**
 * Given the registry
 * When `set` is given one word instead of two
 * Then the line is refused and the message shows the expected shape
 */
TEST_F(ConfigurationRegistryFixture, SetWithOneWordIsRefused) {
  const auto result = interpreter_.execute("set port");
  EXPECT_FALSE(result.accepted);
  EXPECT_NE(result.message.find("expected: set <setting> <value>"),
            std::string::npos);
}

/**
 * Given a setting that may hold nothing and holds a number
 * When `set seed none` runs
 * Then the setting holds nothing
 */
TEST_F(ConfigurationRegistryFixture, NoneClearsASettingThatAllowsIt) {
  set(SEED_SETTING, SEED_TEXT);
  EXPECT_TRUE(set(SEED_SETTING, CLEARING_WORD).accepted);
  EXPECT_FALSE(seed_->integer().has_value());
}

/**
 * Given a port setting, which does not allow nothing
 * When `set port none` runs
 * Then the line is refused and the old value stays
 */
TEST_F(ConfigurationRegistryFixture, NoneIsRefusedWhereNotAllowed) {
  EXPECT_FALSE(set(PORT_SETTING, CLEARING_WORD).accepted);
  EXPECT_EQ(port_->integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given a text setting with a validator
 * When `set nickname` gives a value the validator accepts
 * Then the setting holds the text
 */
TEST_F(ConfigurationRegistryFixture, AcceptedTextIsKept) {
  EXPECT_TRUE(set(NICKNAME_SETTING, ACCEPTED_NICKNAME).accepted);
  EXPECT_EQ(nickname_->text(), ACCEPTED_NICKNAME);
}

/**
 * Given a text setting with a validator
 * When `set nickname` gives a value the validator refuses
 * Then the line is refused with the reason and the old text stays
 */
TEST_F(ConfigurationRegistryFixture, RefusedTextKeepsTheOldOneWithTheReason) {
  const auto result = set(NICKNAME_SETTING, REFUSED_NICKNAME);
  EXPECT_FALSE(result.accepted);
  EXPECT_NE(result.message.find(std::string(NICKNAME_REFUSAL)),
            std::string::npos);
  EXPECT_EQ(nickname_->text(), DEFAULT_NICKNAME);
}

/**
 * Given a declared setting
 * When a setting is declared under the same name
 * Then the declaration is refused with a DuplicateSettingException
 */
TEST_F(ConfigurationRegistryFixture,
       SecondDeclarationWithTheSameNameIsRefused) {
  EXPECT_THROW(registry_.declareInteger(
                   IntegerSettingDefinition{.name = std::string(PORT_SETTING)}),
               DuplicateSettingException);
}
