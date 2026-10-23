#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "BlankRenderTarget.hpp"
#include "BoundAction.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionStatus.hpp"
#include "DrawJournal.hpp"
#include "EffectConstants.hpp"
#include "EffectLengths.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "FakeEntityMessageSource.hpp"
#include "GameScreen.hpp"
#include "GameScreenParts.hpp"
#include "IEntityLayer.hpp"
#include "IScreen.hpp"
#include "KeyPress.hpp"
#include "LobbyState.hpp"
#include "PlayerSettings.hpp"
#include "RecordingEntityLayer.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "RecordingScreen.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSky.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

using rtype::client::BoundAction;
using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::ConnectionStatus;
using rtype::client::GameScreen;
using rtype::client::GameScreenParts;
using rtype::client::IEntityLayer;
using rtype::client::IScreen;
using rtype::client::OPTIONS_KEY;
using rtype::client::PLAYER_INPUT_INTERVAL;
using rtype::client::PLAYER_MUZZLE_FLASH_FRAME;
using rtype::client::PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAME;
using rtype::client::PlayerSettings;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;
using rtype::client::testing::APPLIED_TICK;
using rtype::client::testing::BYDO_ID;
using rtype::client::testing::BYDO_POSITION;
using rtype::client::testing::FIRST_POSITION;
using rtype::client::testing::killed;
using rtype::client::testing::listed;
using rtype::client::testing::PLAYER_MISSILE_ID;
using rtype::client::testing::removed;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::SHIP_POSITION;
using rtype::client::testing::spawned;
using rtype::client::testing::stateListing;
using rtype::client::testing::stateListingPlayers;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::network::LobbyPlayer;
using rtype::network::PlayerState;
using rtype::network::WireEntityType;
using rtype::network::WorldState;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr sf::Keyboard::Key OTHER_KEY = sf::Keyboard::Key::Space;
constexpr int KEY_REPEATS = 3;
constexpr int PAINTS_AT_CONSTRUCTION = 1;
constexpr int PAINTS_AFTER_ONE_UPDATE = 2;
constexpr int SHOWS_AFTER_ONE_DRAW = 1;
constexpr std::size_t ENTITIES_LISTED = 2;
constexpr std::size_t ENTITIES_LEFT_AFTER_ONE_REMOVAL = 1;
constexpr std::uint8_t NOTHING_HELD = 0;
constexpr std::uint8_t UP_BIT = 0b00001;
constexpr std::uint8_t SHOOT_BIT = 0b10000;
constexpr Duration LONG_FRAME = std::chrono::milliseconds(500);
constexpr std::uint8_t FIRST_PILOT = 1;
constexpr std::uint8_t SECOND_PILOT = 2;
constexpr std::uint8_t THIRD_PILOT = 3;
constexpr std::uint8_t LOCAL_NUMBER = SECOND_PILOT;
constexpr std::uint8_t PILOT_LIVES = 3;
constexpr std::uint32_t PILOT_SCORE = 12400;
constexpr int DISPLAYS_AT_CONSTRUCTION = 1;
constexpr float WARP_TOLERANCE = 1e-4F;

/** @brief Accepts a connection as player 2, NOVA, of a waiting room where ACE
 * is player 1 and KAI player 3, none ready. */
void seatThreePilots(RecordingServerConnection& connection) {
  connection.setStatus({.phase = ConnectionStatus::Phase::Accepted,
                        .playerNumber = LOCAL_NUMBER});
  connection.setLobbyPlayers(
      {LobbyPlayer{
           .playerNumber = FIRST_PILOT, .ready = false, .nickname = "ACE"},
       LobbyPlayer{
           .playerNumber = SECOND_PILOT, .ready = false, .nickname = "NOVA"},
       LobbyPlayer{
           .playerNumber = THIRD_PILOT, .ready = false, .nickname = "KAI"}});
}

/** @returns A world state of that tick listing the first pilot with its
 * score and lives, and no entity. */
WorldState stateScoringTheFirstPilot(std::uint32_t tick) {
  return stateListingPlayers(tick, {PlayerState{.playerNumber = FIRST_PILOT,
                                                .shipEntityId = SHIP_ID,
                                                .lives = PILOT_LIVES,
                                                .score = PILOT_SCORE,
                                                .invulnerable = false}});
}

/** @brief An entity layer that ignores every request, for a game screen whose
 * drawing the test does not read. */
class IgnoringEntityLayer : public IEntityLayer {
 public:
  void paint([[maybe_unused]] ComponentRegistry& components,
             [[maybe_unused]] sf::Vector2i offset) override {}
  void show([[maybe_unused]] sf::RenderTarget& target) const override {}
};

/** @brief What a game screen alone is built from, every part a double, the
 * connection accepted as player 2 of a waiting room of three. */
struct LoneGameParts {
  LoneGameParts() { seatThreePilots(connection); }

  RecordingNavigator navigator;
  PlayerSettings settings;
  RecordingPlayerInputLink inputs;
  FakeEntityMessageSource entityMessages;
  RecordingServerConnection connection;
  InterfaceCanvasRecord statusCanvas;
  RecordingSky sky;

  /** @returns The parts of a game screen, borrowing these doubles. */
  [[nodiscard]] GameScreenParts parts() {
    return GameScreenParts{.navigator = navigator,
                           .settings = settings,
                           .inputs = inputs,
                           .entityMessages = entityMessages,
                           .connection = connection,
                           .sky = sky};
  }

  /** @returns A status canvas recording into statusCanvas. */
  [[nodiscard]] std::unique_ptr<RecordingInterfaceCanvas> statusCanvasDouble() {
    return std::make_unique<RecordingInterfaceCanvas>(statusCanvas);
  }
};

/** @brief The real game screen alone in a stack whose other screens are
 * recording doubles, with a recording entity layer, a recording status
 * canvas, a source of entity messages the test fills, and a connection
 * accepted as player 2 of a waiting room of three. */
class GameScreenTest : public ::testing::Test {
 protected:
  void SetUp() override {
    seatThreePilots(connection_);
    layerRecord_.drawJournal = &drawJournal_;
    statusCanvas_.drawJournal = &drawJournal_;
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

  PlayerSettings& settings() { return settings_; }

  [[nodiscard]] const RecordingSky& sky() const { return sky_; }

  FakeEntityMessageSource& entityMessages() { return entityMessages_; }

  [[nodiscard]] const InterfaceCanvasRecord& statusCanvas() const {
    return statusCanvas_;
  }

  [[nodiscard]] const DrawJournal& drawJournal() const { return drawJournal_; }

 private:
  std::vector<JournalEntry> journal_;
  EntityLayerRecord layerRecord_;
  PlayerSettings settings_;
  RecordingPlayerInputLink inputs_;
  FakeEntityMessageSource entityMessages_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord statusCanvas_;
  DrawJournal drawJournal_;
  RecordingSky sky_;
  ScreenStack screens_{[this](ScreenId screen) -> std::unique_ptr<IScreen> {
    if (screen == ScreenId::Game) {
      return std::make_unique<GameScreen>(
          GameScreenParts{.navigator = screens_,
                          .settings = settings_,
                          .inputs = inputs_,
                          .entityMessages = entityMessages_,
                          .connection = connection_,
                          .sky = sky_},
          std::make_unique<RecordingEntityLayer>(layerRecord_),
          std::make_unique<RecordingInterfaceCanvas>(statusCanvas_));
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

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::GameOptions,
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
 * Then its entity layer is already painted, empty until the server announces
 * an entity
 */
TEST_F(GameScreenTest, ANewGameIsPaintedBeforeItsFirstUpdate) {
  EXPECT_EQ(layerRecord().paintCount, PAINTS_AT_CONSTRUCTION);
  EXPECT_THAT(layerRecord().framesAtLastPaint, IsEmpty());
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
 * Given the game alone in the stack, and a WorldState of the server listing a
 * ship and a Bydo
 * When the screens are updated
 * Then both entities are painted
 */
TEST_F(GameScreenTest, TheEntitiesAWorldStateListsArePainted) {
  entityMessages().queue(stateListing(
      APPLIED_TICK, {listed(SHIP_ID, WireEntityType::PlayerShip, SHIP_POSITION),
                     listed(BYDO_ID, WireEntityType::Bydo, BYDO_POSITION)}));

  screens().update(ONE_FRAME);

  EXPECT_EQ(layerRecord().framesAtLastPaint.size(), ENTITIES_LISTED);
}

/**
 * Given the game with a ship and a Bydo the server announced
 * When the server removes the Bydo
 * Then only the ship is painted
 */
TEST_F(GameScreenTest, AnEntityTheServerRemovesIsNoLongerPainted) {
  entityMessages().queue(
      spawned(SHIP_ID, WireEntityType::PlayerShip, SHIP_POSITION));
  entityMessages().queue(spawned(BYDO_ID, WireEntityType::Bydo, BYDO_POSITION));
  screens().update(ONE_FRAME);

  entityMessages().queue(removed(BYDO_ID));
  screens().update(ONE_FRAME);

  EXPECT_EQ(layerRecord().framesAtLastPaint.size(),
            ENTITIES_LEFT_AFTER_ONE_REMOVAL);
}

/**
 * Given the game with a Bydo the server announced
 * When the server says the Bydo was killed
 * Then the Bydo is gone and its explosion is painted
 */
TEST_F(GameScreenTest, AKilledEntityLeavesItsExplosion) {
  entityMessages().queue(spawned(BYDO_ID, WireEntityType::Bydo, BYDO_POSITION));
  entityMessages().queue(killed(BYDO_ID));

  screens().update(ONE_FRAME);

  EXPECT_THAT(layerRecord().framesAtLastPaint,
              ElementsAre(BYDO_EXPLOSION_FRAME));
}

/**
 * Given the explosion of a killed Bydo, just painted
 * When the game plays its freeze and its length, frame by frame
 * Then the explosion has ended and nothing is painted
 */
TEST_F(GameScreenTest, AnExplosionEndsAfterItsLength) {
  entityMessages().queue(spawned(BYDO_ID, WireEntityType::Bydo, BYDO_POSITION));
  entityMessages().queue(killed(BYDO_ID));
  screens().update(ONE_FRAME);

  for (Duration played{}; played < KILL_HOLD_LENGTH + BYDO_EXPLOSION_LENGTH;
       played += ONE_FRAME) {
    screens().update(ONE_FRAME);
  }

  EXPECT_THAT(layerRecord().framesAtLastPaint, IsEmpty());
}

/**
 * Given a player missile the server announced
 * When the next update is one long frame of 500 ms, as after the window was
 * dragged
 * Then the muzzle flash born in that frame is painted, though it lasts 100 ms
 */
TEST_F(GameScreenTest, AnEffectBornInALongFrameIsPaintedOnce) {
  entityMessages().queue(spawned(
      PLAYER_MISSILE_ID, WireEntityType::PlayerMissile, FIRST_POSITION));

  screens().update(LONG_FRAME);

  EXPECT_THAT(layerRecord().framesAtLastPaint,
              Contains(PLAYER_MUZZLE_FLASH_FRAME));
}

/**
 * Given the game running, and the player who turns REDUCE FLASHES on in the
 * options
 * When the server announces a player missile
 * Then its muzzle flash shows its version without flash
 */
TEST_F(GameScreenTest, ReducingFlashesDuringTheGameAppliesToTheNextEffect) {
  settings().reduceFlashes = true;
  entityMessages().queue(spawned(
      PLAYER_MISSILE_ID, WireEntityType::PlayerMissile, FIRST_POSITION));

  screens().update(ONE_FRAME);

  EXPECT_THAT(layerRecord().framesAtLastPaint,
              Contains(PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAME));
}

/**
 * Given a game started from a waiting room where ACE is player 1, NOVA player
 * 2 and KAI player 3
 * When the game is built
 * Then the status band is titled ACE, NOVA and KAI, and no bay by its number
 */
TEST_F(GameScreenTest, TheStatusBandShowsThePilotsOfTheWaitingRoom) {
  EXPECT_NE(writingOf(statusCanvas(), "ACE"), nullptr);
  EXPECT_NE(writingOf(statusCanvas(), "NOVA"), nullptr);
  EXPECT_NE(writingOf(statusCanvas(), "KAI"), nullptr);
  EXPECT_EQ(writingOf(statusCanvas(), "P1"), nullptr);
}

/**
 * Given a game where this client is player 2
 * When the game is built
 * Then the status band says YOU
 */
TEST_F(GameScreenTest, TheStatusBandTagsTheLocalPlayer) {
  EXPECT_NE(writingOf(statusCanvas(), "YOU"), nullptr);
}

/**
 * Given the game alone in the stack
 * When it is drawn once
 * Then the status band is laid once
 */
TEST_F(GameScreenTest, DrawingLaysTheStatusBand) {
  BlankRenderTarget target;

  screens().draw(target);

  EXPECT_EQ(statusCanvas().showCount, SHOWS_AFTER_ONE_DRAW);
}

/**
 * Given the game alone in the stack
 * When it is drawn
 * Then the status band is laid after the entities, so no entity covers it
 */
TEST_F(GameScreenTest, TheStatusBandIsLaidOverTheEntities) {
  BlankRenderTarget target;

  screens().draw(target);

  EXPECT_THAT(drawJournal().laid,
              ElementsAre(ENTITY_LAYER_LAID, INTERFACE_CANVAS_LAID));
}

/**
 * Given the game
 * When a world state gives player 1 12400 points and a frame passes
 * Then the status band shows 12400
 */
TEST_F(GameScreenTest, AWorldStateRepaintsTheStatusBand) {
  entityMessages().queue(stateScoringTheFirstPilot(APPLIED_TICK));

  screens().update(ONE_FRAME);

  EXPECT_NE(writingOf(statusCanvas(), "12400"), nullptr);
}

/**
 * Given the game
 * When a world state that lists no player arrives and a frame passes
 * Then the status band is not painted again
 */
TEST_F(GameScreenTest, AStateThatChangesNoPilotDoesNotRepaint) {
  entityMessages().queue(stateListing(APPLIED_TICK, {}));

  screens().update(ONE_FRAME);

  EXPECT_EQ(statusCanvas().displayCount, DISPLAYS_AT_CONSTRUCTION);
}

/**
 * Given the game screen
 * When the player presses up and space
 * Then both actions are held
 */
TEST(GameScreenActionsTest, PressedKeysBecomeHeldActions) {
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());

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
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
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
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
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
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
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
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
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
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  game.handleEvent(keyRelease(OPTIONS_KEY, sf::Keyboard::Scan::Escape));

  EXPECT_EQ(game.playerActions().bits(), NOTHING_HELD);
}

/**
 * Given move up moved to W in the options
 * When W is pressed in the game
 * Then up is held
 */
TEST(GameScreenActionsTest, AKeyChangedInTheOptionsMovesTheShip) {
  LoneGameParts doubles;
  doubles.settings.keys.bind(BoundAction::MoveUp, sf::Keyboard::Scan::W);
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());

  game.handleEvent(keyPress(sf::Keyboard::Key::W, sf::Keyboard::Scan::W));

  EXPECT_EQ(game.playerActions().bits(), UP_BIT);
}

/**
 * Given the game with up held
 * When its first frame is updated
 * Then an input holding up is sent at once
 */
TEST(GameScreenInputsTest, TheFirstUpdateSendsTheActionsHeld) {
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  game.update(ONE_FRAME);

  EXPECT_THAT(doubles.inputs.sentActions(), ElementsAre(UP_BIT));
}

/**
 * Given the game after a first update of 16 ms, which sent an input
 * When an update of one nanosecond follows, still within the first interval
 * Then no other input is sent: inputs follow the time, not the updates
 */
TEST(GameScreenInputsTest, AnUpdateWithinTheIntervalSendsNothingMore) {
  ASSERT_LT(ONE_FRAME + ONE_TICK, PLAYER_INPUT_INTERVAL);
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.update(ONE_FRAME);

  game.update(ONE_TICK);

  EXPECT_EQ(doubles.inputs.sentActions().size(), 1);
}

/**
 * Given the game with up held, and the options just opened over it
 * When the game is updated behind them
 * Then it sends an input with nothing held
 */
TEST(GameScreenInputsTest, BehindTheOptionsItSendsNothingHeld) {
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));
  game.handleEvent(keyRelease(OPTIONS_KEY, sf::Keyboard::Scan::Escape));

  game.update(ONE_FRAME);

  EXPECT_THAT(doubles.inputs.sentActions(), ElementsAre(NOTHING_HELD));
}

/**
 * Given a game just started
 * When 117 ms pass
 * Then the stars of the sky go 12 times their speed
 */
TEST_F(GameScreenTest, TheStarsWarpAtTheStartOfTheGame) {
  screens().update(WARP_TOP_TIME);

  EXPECT_NEAR(sky().lastSpeedFactor(), TOP_WARP_SPEED, WARP_TOLERANCE);
}

/**
 * Given a game just started
 * When 600 ms pass
 * Then the stars go their own speed again
 */
TEST_F(GameScreenTest, TheStarsSlowDownToTheirSpeed) {
  screens().update(WARP_END_TIME);

  EXPECT_NEAR(sky().lastSpeedFactor(), OWN_SPEED, WARP_TOLERANCE);
}

/**
 * Given a player who reduced flashes
 * When the game starts and 117 ms pass
 * Then the stars keep their own speed
 */
TEST_F(GameScreenTest, WithFlashesReducedTheStarsDoNotWarp) {
  settings().reduceFlashes = true;

  screens().update(WARP_TOP_TIME);

  EXPECT_EQ(sky().lastSpeedFactor(), OWN_SPEED);
}

/**
 * Given a game whose stars warp
 * When the game leaves the stack
 * Then the sky goes its own speed, in its place
 */
TEST_F(GameScreenTest, LeavingTheGamePutsTheSkyBack) {
  screens().update(WARP_TOP_TIME);

  screens().pop();
  screens().applyPendingChanges();

  EXPECT_EQ(sky().lastSpeedFactor(), OWN_SPEED);
  EXPECT_EQ(sky().lastOffset(), sf::Vector2i());
}

/**
 * Given a game just started
 * When 1 s passes
 * Then the status canvas is written "M", the first letter of the banner
 */
TEST_F(GameScreenTest, TheBannerStartsAtOneSecond) {
  screens().update(BANNER_START_TIME);

  EXPECT_NE(writingOf(statusCanvas(), "M"), nullptr);
}

/**
 * Given a game at rest, no trauma
 * When a frame passes
 * Then the entity layer is painted in its place
 */
TEST_F(GameScreenTest, TheEntitiesArePaintedInPlaceAtRest) {
  screens().update(ONE_FRAME);

  EXPECT_EQ(layerRecord().offsetAtLastPaint, sf::Vector2i());
}
