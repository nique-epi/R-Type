#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include <vector>
#include "BlankRenderTarget.hpp"
#include "BoundAction.hpp"
#include "ComponentRegistry.hpp"
#include "GameScreen.hpp"
#include "IEntityLayer.hpp"
#include "IScreen.hpp"
#include "KeyBindings.hpp"
#include "KeyPress.hpp"
#include "RecordingEntityLayer.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingScreen.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "ScreenTestConstants.hpp"
#include "SpriteShowcaseConstants.hpp"

using rtype::client::BoundAction;
using rtype::client::GameScreen;
using rtype::client::IEntityLayer;
using rtype::client::IScreen;
using rtype::client::KeyBindings;
using rtype::client::OPTIONS_KEY;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;
using rtype::client::SPRITE_SHOWCASE;
using rtype::engine::ComponentRegistry;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr sf::Keyboard::Key OTHER_KEY = sf::Keyboard::Key::Space;
constexpr int KEY_REPEATS = 3;
constexpr int PAINTS_AT_CONSTRUCTION = 1;
constexpr int PAINTS_AFTER_ONE_UPDATE = 2;
constexpr int SHOWS_AFTER_ONE_DRAW = 1;
constexpr std::uint8_t NOTHING_HELD = 0;
constexpr std::uint8_t UP_BIT = 0b00001;
constexpr std::uint8_t SHOOT_BIT = 0b10000;

/** @brief An entity layer that ignores every request, for a game screen whose
 * drawing the test does not read. */
class IgnoringEntityLayer : public IEntityLayer {
 public:
  void paint([[maybe_unused]] ComponentRegistry& components) override {}
  void show([[maybe_unused]] sf::RenderTarget& target) const override {}
};

/** @brief The real game screen alone in a stack whose other screens are
 * recording doubles, with a recording entity layer. */
class GameScreenTest : public ::testing::Test {
 protected:
  void SetUp() override {
    screens_.push(ScreenId::Game);
    screens_.applyPendingChanges();
  }

  ScreenStack& screens() { return screens_; }

  [[nodiscard]] const std::vector<JournalEntry>& journal() const {
    return journal_;
  }

  [[nodiscard]] const EntityLayerRecord& layerRecord() const {
    return layerRecord_;
  }

 private:
  std::vector<JournalEntry> journal_;
  EntityLayerRecord layerRecord_;
  KeyBindings keys_;
  ScreenStack screens_{[this](ScreenId screen) -> std::unique_ptr<IScreen> {
    if (screen == ScreenId::Game) {
      return std::make_unique<GameScreen>(
          screens_, std::make_unique<RecordingEntityLayer>(layerRecord_),
          keys_);
    }
    return std::make_unique<RecordingScreen>(screen, journal_);
  }};
};

}  // namespace

/**
 * Given the game alone in the stack
 * When the player releases the options key
 * Then the options screen is built
 */
TEST_F(GameScreenTest, ReleasingTheOptionsKeyOpensTheOptions) {
  screens().handleEvent(keyRelease(OPTIONS_KEY));
  screens().applyPendingChanges();

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = ScreenId::Options,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given the options opened over the game
 * When the options leave the stack
 * Then the game is still there
 */
TEST_F(GameScreenTest, OptionsOpenOverTheGame) {
  screens().handleEvent(keyRelease(OPTIONS_KEY));
  screens().applyPendingChanges();

  screens().pop();
  screens().applyPendingChanges();

  EXPECT_FALSE(screens().isEmpty());
}

/**
 * Given the game alone in the stack
 * When the player holds the options key down, the system repeating its press
 * Then no options screen is built
 */
TEST_F(GameScreenTest, HoldingTheOptionsKeyLeavesTheOptionsClosed) {
  for (int repeat = 0; repeat < KEY_REPEATS; ++repeat) {
    screens().handleEvent(keyPress(OPTIONS_KEY));
    screens().applyPendingChanges();
  }

  EXPECT_THAT(journal(), IsEmpty());
}

/**
 * Given the game alone in the stack
 * When the player releases another key
 * Then no options screen is built
 */
TEST_F(GameScreenTest, OtherKeysLeaveTheOptionsClosed) {
  screens().handleEvent(keyRelease(OTHER_KEY));
  screens().applyPendingChanges();

  EXPECT_THAT(journal(), IsEmpty());
}

/**
 * Given a game screen just built
 * When nothing has updated it yet
 * Then its entity layer is already painted with every entity of the showcase
 */
TEST_F(GameScreenTest, ANewGameIsPaintedBeforeItsFirstUpdate) {
  EXPECT_EQ(layerRecord().paintCount, PAINTS_AT_CONSTRUCTION);
  EXPECT_EQ(layerRecord().spritesAtLastPaint, SPRITE_SHOWCASE.size());
}

/**
 * Given the game alone in the stack
 * When the screens are updated
 * Then the entity layer is painted again
 */
TEST_F(GameScreenTest, EachUpdateRepaintsTheEntityLayer) {
  screens().update(ONE_FRAME);

  EXPECT_EQ(layerRecord().paintCount, PAINTS_AFTER_ONE_UPDATE);
}

/**
 * Given the game alone in the stack
 * When the screens are drawn on a target
 * Then the entity layer is laid on that target
 */
TEST_F(GameScreenTest, DrawingLaysTheEntityLayerOnTheTarget) {
  BlankRenderTarget target;

  screens().draw(target);

  EXPECT_EQ(layerRecord().showCount, SHOWS_AFTER_ONE_DRAW);
  EXPECT_EQ(layerRecord().lastShowTarget, &target);
}

/**
 * Given the game screen
 * When the player presses up and space
 * Then both actions are held
 */
TEST(GameScreenActionsTest, PressedKeysBecomeHeldActions) {
  RecordingNavigator navigator;
  const KeyBindings keys;
  GameScreen game(navigator, std::make_unique<IgnoringEntityLayer>(), keys);

  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));
  game.handleEvent(
      keyPress(sf::Keyboard::Key::Space, sf::Keyboard::Scan::Space));

  EXPECT_EQ(game.playerActions().bits(), UP_BIT | SHOOT_BIT);
}

/**
 * Given the game screen with up held
 * When the player releases up
 * Then nothing is held
 */
TEST(GameScreenActionsTest, ReleasedKeyIsNoLongerHeld) {
  RecordingNavigator navigator;
  const KeyBindings keys;
  GameScreen game(navigator, std::make_unique<IgnoringEntityLayer>(), keys);
  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  game.handleEvent(keyRelease(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  EXPECT_EQ(game.playerActions().bits(), NOTHING_HELD);
}

/**
 * Given the game screen with up held
 * When the window loses the focus
 * Then nothing is held
 */
TEST(GameScreenActionsTest, LosingFocusReleasesTheHeldActions) {
  RecordingNavigator navigator;
  const KeyBindings keys;
  GameScreen game(navigator, std::make_unique<IgnoringEntityLayer>(), keys);
  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  game.handleEvent(sf::Event{sf::Event::FocusLost{}});

  EXPECT_EQ(game.playerActions().bits(), NOTHING_HELD);
}

/**
 * Given the game screen after the window lost the focus
 * When the player presses up
 * Then nothing is held
 */
TEST(GameScreenActionsTest, KeysPressedWithoutFocusAreIgnored) {
  RecordingNavigator navigator;
  const KeyBindings keys;
  GameScreen game(navigator, std::make_unique<IgnoringEntityLayer>(), keys);
  game.handleEvent(sf::Event{sf::Event::FocusLost{}});

  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  EXPECT_EQ(game.playerActions().bits(), NOTHING_HELD);
}

/**
 * Given the game screen after the window lost and regained the focus
 * When the player presses up
 * Then up is held
 */
TEST(GameScreenActionsTest, KeysPressedAfterFocusGainedCount) {
  RecordingNavigator navigator;
  const KeyBindings keys;
  GameScreen game(navigator, std::make_unique<IgnoringEntityLayer>(), keys);
  game.handleEvent(sf::Event{sf::Event::FocusLost{}});
  game.handleEvent(sf::Event{sf::Event::FocusGained{}});

  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  EXPECT_EQ(game.playerActions().bits(), UP_BIT);
}

/**
 * Given up held in the game
 * When the options key is released
 * Then nothing is held any more
 */
TEST(GameScreenActionsTest, OpeningTheOptionsReleasesTheHeldActions) {
  RecordingNavigator navigator;
  const KeyBindings keys;
  GameScreen game(navigator, std::make_unique<IgnoringEntityLayer>(), keys);
  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));
  game.handleEvent(keyRelease(OPTIONS_KEY, sf::Keyboard::Scan::Escape));
  EXPECT_EQ(game.playerActions().bits(), 0U);
}

/**
 * Given move up moved to W in the options
 * When W is pressed in the game
 * Then up is held
 */
TEST(GameScreenActionsTest, AKeyChangedInTheOptionsMovesTheShip) {
  RecordingNavigator navigator;
  KeyBindings keys;
  keys.bind(BoundAction::MoveUp, sf::Keyboard::Scan::W);
  GameScreen game(navigator, std::make_unique<IgnoringEntityLayer>(), keys);
  game.handleEvent(keyPress(sf::Keyboard::Key::W, sf::Keyboard::Scan::W));
  EXPECT_EQ(game.playerActions().bits(), UP_BIT);
}
