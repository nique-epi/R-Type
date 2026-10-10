#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include "BlankRenderTarget.hpp"
#include "KeyPress.hpp"
#include "RecordingScreen.hpp"
#include "ScreenStack.hpp"

using rtype::client::ScreenFactory;
using rtype::client::ScreenStack;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr const char* GAME = "game";
constexpr const char* OPTIONS = "options";
constexpr const char* NEXT = "next";
constexpr sf::Keyboard::Key ANY_KEY = sf::Keyboard::Key::A;

ScreenFactory screenRemovingItselfOnEvent(ScreenStack& screens,
                                          std::vector<JournalEntry>& journal) {
  return [&screens, &journal] {
    auto screen = std::make_unique<RecordingScreen>(GAME, journal);
    screen->runOnEvent([&screens] { screens.pop(); });
    return screen;
  };
}

}  // namespace

/**
 * Given the options pushed over the game
 * When an event arrives
 * Then only the options receive it
 */
TEST(ScreenStack, EventsReachOnlyTheScreenOnTop) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(recordingScreen(GAME, journal));
  screens.push(recordingScreen(OPTIONS, journal));
  screens.applyPendingChanges();
  journal.clear();

  screens.handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal,
              ElementsAre(JournalEntry{.screen = OPTIONS,
                                       .call = ScreenCall::HandleEvent}));
}

/**
 * Given the options pushed over the game
 * When a frame is updated
 * Then the game is updated too, so it keeps running behind the options
 */
TEST(ScreenStack, EveryScreenIsUpdatedFromTheBottomUp) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(recordingScreen(GAME, journal));
  screens.push(recordingScreen(OPTIONS, journal));
  screens.applyPendingChanges();
  journal.clear();

  screens.update();

  EXPECT_THAT(
      journal,
      ElementsAre(JournalEntry{.screen = GAME, .call = ScreenCall::Update},
                  JournalEntry{.screen = OPTIONS, .call = ScreenCall::Update}));
}

/**
 * Given the options pushed over the game
 * When a frame is drawn
 * Then the game is drawn first and the options over it
 */
TEST(ScreenStack, EveryScreenIsDrawnFromTheBottomUp) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  BlankRenderTarget target;
  screens.push(recordingScreen(GAME, journal));
  screens.push(recordingScreen(OPTIONS, journal));
  screens.applyPendingChanges();
  journal.clear();

  screens.draw(target);

  EXPECT_THAT(
      journal,
      ElementsAre(JournalEntry{.screen = GAME, .call = ScreenCall::Draw},
                  JournalEntry{.screen = OPTIONS, .call = ScreenCall::Draw}));
}

/**
 * Given the options pushed over the game
 * When the options are popped and an event arrives
 * Then the game receives it
 */
TEST(ScreenStack, PoppingGivesTheEventsBackToTheScreenBelow) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(recordingScreen(GAME, journal));
  screens.push(recordingScreen(OPTIONS, journal));
  screens.applyPendingChanges();
  screens.pop();
  screens.applyPendingChanges();
  journal.clear();

  screens.handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal, ElementsAre(JournalEntry{
                           .screen = GAME, .call = ScreenCall::HandleEvent}));
}

/**
 * Given a screen pushed but not applied yet
 * When an event arrives
 * Then the screen is neither built nor given the event
 */
TEST(ScreenStack, AskedChangeWaitsUntilApplied) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(recordingScreen(GAME, journal));

  screens.handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal, IsEmpty());
}

/**
 * Given a screen that asks to be popped while it handles an event
 * When it handles an event
 * Then it is still alive once its handling returns
 */
TEST(ScreenStack, ScreenAskingToLeaveStaysAliveWhileHandlingAnEvent) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(screenRemovingItselfOnEvent(screens, journal));
  screens.applyPendingChanges();
  journal.clear();

  screens.handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal, ElementsAre(JournalEntry{
                           .screen = GAME, .call = ScreenCall::HandleEvent}));
}

/**
 * Given the options pushed over the game
 * When every screen is replaced by a new one
 * Then the options then the game are destroyed before the new one is built
 */
TEST(ScreenStack, ReplacingDestroysEveryScreenBeforeBuildingTheNewOne) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(recordingScreen(GAME, journal));
  screens.push(recordingScreen(OPTIONS, journal));
  screens.applyPendingChanges();
  journal.clear();

  screens.replaceAll(recordingScreen(NEXT, journal));
  screens.applyPendingChanges();

  EXPECT_THAT(
      journal,
      ElementsAre(
          JournalEntry{.screen = OPTIONS, .call = ScreenCall::Destroyed},
          JournalEntry{.screen = GAME, .call = ScreenCall::Destroyed},
          JournalEntry{.screen = NEXT, .call = ScreenCall::Built}));
}

/**
 * Given a push then a pop asked for before applying
 * When the changes are applied
 * Then they are applied in that order and the stack is empty
 */
TEST(ScreenStack, ChangesAreAppliedInTheOrderAskedFor) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(recordingScreen(GAME, journal));
  screens.pop();

  screens.applyPendingChanges();

  EXPECT_TRUE(screens.isEmpty());
}

/**
 * Given an empty stack
 * When a pop is applied
 * Then the stack stays empty
 */
TEST(ScreenStack, PoppingAnEmptyStackLeavesItEmpty) {
  ScreenStack screens;
  screens.pop();

  screens.applyPendingChanges();

  EXPECT_TRUE(screens.isEmpty());
}

/**
 * Given a screen whose building asks for the options to be pushed
 * When the changes are applied once and an event arrives
 * Then the options are not built yet and the screen receives the event
 */
TEST(ScreenStack, ChangeAskedWhileApplyingWaitsForTheNextApply) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push([&screens, &journal] {
    screens.push(recordingScreen(OPTIONS, journal));
    return std::make_unique<RecordingScreen>(GAME, journal);
  });
  screens.applyPendingChanges();

  screens.handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(
      journal,
      ElementsAre(
          JournalEntry{.screen = GAME, .call = ScreenCall::Built},
          JournalEntry{.screen = GAME, .call = ScreenCall::HandleEvent}));
}
