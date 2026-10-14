#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include "BlankRenderTarget.hpp"
#include "KeyPress.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::ScreenId;
using rtype::client::ScreenStack;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr sf::Keyboard::Key ANY_KEY = sf::Keyboard::Key::A;

/** @brief A stack whose factory builds a recording screen for every
 * identifier, after letting the test act on the game screen it builds. */
class ScreenStackTest : public ::testing::Test {
 protected:
  void showOptionsOverTheGame() {
    screens_.push(ScreenId::Game);
    screens_.push(ScreenId::Options);
    screens_.applyPendingChanges();
    journal_.clear();
  }

  ScreenStack& screens() { return screens_; }

  std::vector<JournalEntry>& journal() { return journal_; }

  void onGameBuilt(std::function<void(RecordingScreen&)> action) {
    onGameBuilt_ = std::move(action);
  }

 private:
  std::vector<JournalEntry> journal_;
  std::function<void(RecordingScreen&)> onGameBuilt_;
  ScreenStack screens_{[this](ScreenId screen) {
    auto built = std::make_unique<RecordingScreen>(screen, journal_);
    if (screen == ScreenId::Game && onGameBuilt_ != nullptr) {
      onGameBuilt_(*built);
    }
    return built;
  }};
};

}  // namespace

/**
 * Given the options pushed over the game
 * When an event arrives
 * Then only the options receive it
 */
TEST_F(ScreenStackTest, EventsReachOnlyTheScreenOnTop) {
  showOptionsOverTheGame();

  screens().handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::Options,
                                       .call = ScreenCall::HandleEvent}));
}

/**
 * Given the options pushed over the game
 * When a frame is updated
 * Then the game is updated too, so it keeps running behind the options
 */
TEST_F(ScreenStackTest, EveryScreenIsUpdatedFromTheBottomUp) {
  showOptionsOverTheGame();

  screens().update(ONE_FRAME);

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = ScreenId::Game,
                                                  .call = ScreenCall::Update},
                                     JournalEntry{.screen = ScreenId::Options,
                                                  .call = ScreenCall::Update}));
}

/**
 * Given the options pushed over the game
 * When a frame is drawn
 * Then the game is drawn first and the options over it
 */
TEST_F(ScreenStackTest, EveryScreenIsDrawnFromTheBottomUp) {
  BlankRenderTarget target;
  showOptionsOverTheGame();

  screens().draw(target);

  EXPECT_THAT(
      journal(),
      ElementsAre(
          JournalEntry{.screen = ScreenId::Game, .call = ScreenCall::Draw},
          JournalEntry{.screen = ScreenId::Options, .call = ScreenCall::Draw}));
}

/**
 * Given the options pushed over the game
 * When the options are popped and an event arrives
 * Then the game receives it
 */
TEST_F(ScreenStackTest, PoppingGivesTheEventsBackToTheScreenBelow) {
  showOptionsOverTheGame();
  screens().pop();
  screens().applyPendingChanges();
  journal().clear();

  screens().handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::Game,
                                       .call = ScreenCall::HandleEvent}));
}

/**
 * Given a screen pushed but not applied yet
 * When an event arrives
 * Then the screen is neither built nor given the event
 */
TEST_F(ScreenStackTest, AskedChangeWaitsUntilApplied) {
  screens().push(ScreenId::Game);

  screens().handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal(), IsEmpty());
}

/**
 * Given a screen that asks to be popped while it handles an event
 * When it handles an event
 * Then it is still alive once its handling returns
 */
TEST_F(ScreenStackTest, ScreenAskingToLeaveStaysAliveWhileHandlingAnEvent) {
  onGameBuilt([this](RecordingScreen& game) {
    game.runOnEvent([this] { screens().pop(); });
  });
  screens().push(ScreenId::Game);
  screens().applyPendingChanges();
  journal().clear();

  screens().handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::Game,
                                       .call = ScreenCall::HandleEvent}));
}

/**
 * Given the options pushed over the game
 * When every screen is replaced by a new game
 * Then the options then the game are destroyed before the new game is built
 */
TEST_F(ScreenStackTest, ReplacingDestroysEveryScreenBeforeBuildingTheNewOne) {
  showOptionsOverTheGame();

  screens().replaceAll(ScreenId::Game);
  screens().applyPendingChanges();

  EXPECT_THAT(
      journal(),
      ElementsAre(
          JournalEntry{.screen = ScreenId::Options,
                       .call = ScreenCall::Destroyed},
          JournalEntry{.screen = ScreenId::Game, .call = ScreenCall::Destroyed},
          JournalEntry{.screen = ScreenId::Game, .call = ScreenCall::Built}));
}

/**
 * Given a push then a pop asked for before applying
 * When the changes are applied
 * Then they are applied in that order and the stack is empty
 */
TEST_F(ScreenStackTest, ChangesAreAppliedInTheOrderAskedFor) {
  screens().push(ScreenId::Game);
  screens().pop();

  screens().applyPendingChanges();

  EXPECT_TRUE(screens().isEmpty());
}

/**
 * Given an empty stack
 * When a pop is applied
 * Then the stack stays empty
 */
TEST_F(ScreenStackTest, PoppingAnEmptyStackLeavesItEmpty) {
  screens().pop();

  screens().applyPendingChanges();

  EXPECT_TRUE(screens().isEmpty());
}

/**
 * Given a game screen whose building asks for the options to be pushed
 * When the changes are applied once and an event arrives
 * Then the options are not built yet and the game receives the event
 */
TEST_F(ScreenStackTest, ChangeAskedWhileApplyingWaitsForTheNextApply) {
  onGameBuilt([this]([[maybe_unused]] RecordingScreen& game) {
    screens().push(ScreenId::Options);
  });
  screens().push(ScreenId::Game);
  screens().applyPendingChanges();

  screens().handleEvent(keyPress(ANY_KEY));

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::Game,
                                       .call = ScreenCall::Built},
                          JournalEntry{.screen = ScreenId::Game,
                                       .call = ScreenCall::HandleEvent}));
}

/**
 * Given the options pushed over the game
 * When a frame of 16 milliseconds is updated
 * Then the game, below, is updated by those 16 milliseconds
 */
TEST_F(ScreenStackTest, EveryScreenIsUpdatedByTheTimeOfTheFrame) {
  RecordingScreen* game = nullptr;
  onGameBuilt([&game](RecordingScreen& built) { game = &built; });
  showOptionsOverTheGame();

  screens().update(ONE_FRAME);

  ASSERT_NE(game, nullptr);
  EXPECT_EQ(game->lastElapsed(), ONE_FRAME);
}

/**
 * Given the options pushed over the game
 * When every screen is asked to close
 * Then the options are destroyed, then the game
 */
TEST_F(ScreenStackTest, CloseAllDestroysEveryScreenFromTheTopDown) {
  showOptionsOverTheGame();

  screens().closeAll();
  screens().applyPendingChanges();

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::Options,
                                       .call = ScreenCall::Destroyed},
                          JournalEntry{.screen = ScreenId::Game,
                                       .call = ScreenCall::Destroyed}));
}

/**
 * Given the options pushed over the game
 * When every screen is asked to close
 * Then no screen is left, which stops the frame loop
 */
TEST_F(ScreenStackTest, CloseAllLeavesNoScreen) {
  showOptionsOverTheGame();

  screens().closeAll();
  screens().applyPendingChanges();

  EXPECT_TRUE(screens().isEmpty());
}
