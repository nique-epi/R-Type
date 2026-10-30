#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include "ClientException.hpp"
#include "IScreen.hpp"
#include "RecordingScreen.hpp"
#include "ScreenRegistryFixture.hpp"
#include "ScreenRegistryTestConstants.hpp"

using rtype::client::DuplicateScreenException;
using rtype::client::IScreen;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a screen registered under an identifier
 * When the screen of that identifier is asked for
 * Then the registry builds it
 */
TEST_F(ScreenRegistryFixture, BuildsTheScreenRegisteredUnderAnIdentifier) {
  registerRecordingScreen(EXTRA_SCREEN);

  const std::unique_ptr<IScreen> screen =
      registry().create(EXTRA_SCREEN, CONTEXT_OF_THE_TEST);

  EXPECT_NE(screen, nullptr);
  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = EXTRA_SCREEN,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given a screen registered under one identifier
 * When another identifier is asked for
 * Then the registry builds nothing
 */
TEST_F(ScreenRegistryFixture, BuildsNothingForAnIdentifierNobodyRegistered) {
  registerRecordingScreen(EXTRA_SCREEN);

  const std::unique_ptr<IScreen> screen =
      registry().create(OTHER_SCREEN, CONTEXT_OF_THE_TEST);

  EXPECT_EQ(screen, nullptr);
  EXPECT_THAT(journal(), IsEmpty());
}

/**
 * Given a screen registered under an identifier
 * When another screen is registered under the same identifier
 * Then the registry refuses it
 */
TEST_F(ScreenRegistryFixture, RefusesASecondScreenUnderTheSameIdentifier) {
  registerRecordingScreen(EXTRA_SCREEN);

  EXPECT_THROW(registerRecordingScreen(EXTRA_SCREEN), DuplicateScreenException);
}

/**
 * Given a builder registered under an identifier
 * When its screen is asked for with a context
 * Then the builder is given that context
 */
TEST_F(ScreenRegistryFixture, GivesTheContextToTheBuilder) {
  int received = 0;
  registry().add(EXTRA_SCREEN, [&received](const int& context) {
    received = context;
    return nullptr;
  });

  const std::unique_ptr<IScreen> screen =
      registry().create(EXTRA_SCREEN, CONTEXT_OF_THE_TEST);

  EXPECT_EQ(received, CONTEXT_OF_THE_TEST);
}

/**
 * Given a screen registered after the stack was made
 * When the stack is asked to push that screen
 * Then the stack builds it through the registry and shows it
 */
TEST_F(ScreenRegistryFixture, StackShowsAScreenAddedToTheRegistry) {
  registerRecordingScreen(EXTRA_SCREEN);

  stack().push(EXTRA_SCREEN);
  stack().applyPendingChanges();

  EXPECT_FALSE(stack().isEmpty());
  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = EXTRA_SCREEN,
                                                  .call = ScreenCall::Built}));
}
