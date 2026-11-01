#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include "BlankRenderTarget.hpp"
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "ConnectionConstants.hpp"
#include "DepartureFeedTestConstants.hpp"
#include "DrawJournal.hpp"
#include "EffectConstants.hpp"
#include "EffectTestConstants.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "FakeEntityMessageSource.hpp"
#include "GameEnded.hpp"
#include "GameResult.hpp"
#include "GameScreen.hpp"
#include "GameScreenFixture.hpp"
#include "GameScreenFixtures.hpp"
#include "GameScreenTestConstants.hpp"
#include "IgnoringEntityLayer.hpp"
#include "KeyPress.hpp"
#include "LoneGameParts.hpp"
#include "PlayerPaletteTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "RecordingEntityLayer.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "RecordingScreen.hpp"
#include "RecordingSky.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "ScreenTestConstants.hpp"
#include "SilenceWarningTestConstants.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

using rtype::client::BoundAction;
using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::ColorMode;
using rtype::client::GameScreen;
using rtype::client::OPTIONS_KEY;
using rtype::client::PLAYER_INPUT_INTERVAL;
using rtype::client::PLAYER_MUZZLE_FLASH_FRAME;
using rtype::client::PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAME;
using rtype::client::PLAYER_SHOT_SOUND_ID;
using rtype::client::ScreenId;
using rtype::client::testing::APPLIED_TICK;
using rtype::client::testing::BYDO_ID;
using rtype::client::testing::BYDO_POSITION;
using rtype::client::testing::FIRST_POSITION;
using rtype::client::testing::killed;
using rtype::client::testing::LIGHT_BLUISH_GREEN;
using rtype::client::testing::listed;
using rtype::client::testing::PLAYER_MISSILE_ID;
using rtype::client::testing::PLAYER_MISSILE_POSITION;
using rtype::client::testing::removed;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::SHIP_POSITION;
using rtype::client::testing::spawned;
using rtype::client::testing::stateListing;
using rtype::engine::Duration;
using rtype::engine::testing::ONE_SECOND;
using rtype::network::WireEntityType;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the game alone in the stack
 * When the player releases the options key
 * Then the options screen is built
 */
TEST_F(GameScreenFixture, ReleasingTheOptionsKeyOpensTheOptions) {
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
TEST_F(GameScreenFixture, OptionsOpenOverTheGame) {
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
TEST_F(GameScreenFixture, HoldingTheOptionsKeyLeavesTheOptionsClosed) {
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
TEST_F(GameScreenFixture, OtherKeysLeaveTheOptionsClosed) {
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
TEST_F(GameScreenFixture, ANewGameIsPaintedBeforeItsFirstUpdate) {
  EXPECT_EQ(layerRecord().paintCount, PAINTS_AT_CONSTRUCTION);
  EXPECT_THAT(layerRecord().framesAtLastPaint, IsEmpty());
}

/**
 * Given the game alone in the stack
 * When the screens are updated
 * Then the entity layer is painted again
 */
TEST_F(GameScreenFixture, EachUpdateRepaintsTheEntityLayer) {
  screens().update(ONE_FRAME);

  EXPECT_EQ(layerRecord().paintCount, PAINTS_AFTER_ONE_UPDATE);
}

/**
 * Given the game alone in the stack
 * When the screens are drawn on a target
 * Then the entity layer is laid on that target
 */
TEST_F(GameScreenFixture, DrawingLaysTheEntityLayerOnTheTarget) {
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
TEST_F(GameScreenFixture, TheEntitiesAWorldStateListsArePainted) {
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
TEST_F(GameScreenFixture, AnEntityTheServerRemovesIsNoLongerPainted) {
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
TEST_F(GameScreenFixture, AKilledEntityLeavesItsExplosion) {
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
TEST_F(GameScreenFixture, AnExplosionEndsAfterItsLength) {
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
TEST_F(GameScreenFixture, AnEffectBornInALongFrameIsPaintedOnce) {
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
TEST_F(GameScreenFixture, ReducingFlashesDuringTheGameAppliesToTheNextEffect) {
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
TEST_F(GameScreenFixture, TheStatusBandShowsThePilotsOfTheWaitingRoom) {
  EXPECT_NE(writingOf(statusCanvas(), FIRST_PILOT_NICKNAME), nullptr);
  EXPECT_NE(writingOf(statusCanvas(), SECOND_PILOT_NICKNAME), nullptr);
  EXPECT_NE(writingOf(statusCanvas(), THIRD_PILOT_NICKNAME), nullptr);
  EXPECT_EQ(writingOf(statusCanvas(), "P1"), nullptr);
}

/**
 * Given a game where this client is player 2
 * When the game is built
 * Then the status band says YOU
 */
TEST_F(GameScreenFixture, TheStatusBandTagsTheLocalPlayer) {
  EXPECT_NE(writingOf(statusCanvas(), "YOU"), nullptr);
}

/**
 * Given the game alone in the stack
 * When it is drawn once
 * Then the status band is laid once
 */
TEST_F(GameScreenFixture, DrawingLaysTheStatusBand) {
  BlankRenderTarget target;

  screens().draw(target);

  EXPECT_EQ(statusCanvas().showCount, SHOWS_AFTER_ONE_DRAW);
}

/**
 * Given the game alone in the stack
 * When it is drawn
 * Then the status band is laid after the entities, so no entity covers it
 */
TEST_F(GameScreenFixture, TheStatusBandIsLaidOverTheEntities) {
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
TEST_F(GameScreenFixture, AWorldStateRepaintsTheStatusBand) {
  entityMessages().queue(stateScoringTheFirstPilot(APPLIED_TICK));

  screens().update(ONE_FRAME);

  EXPECT_NE(writingOf(statusCanvas(), "12400"), nullptr);
}

/**
 * Given the game
 * When a world state that lists no player arrives and a frame passes
 * Then the status band is not painted again
 */
TEST_F(GameScreenFixture, AStateThatChangesNoPilotDoesNotRepaint) {
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
 * Given the game with AUTO-FIRE on and no key pressed
 * When its first frame is updated
 * Then the input sent holds shoot
 */
TEST(GameScreenAutoFireTest, ShootsWithoutAKey) {
  LoneGameParts doubles;
  doubles.settings.autoFire = true;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());

  game.update(ONE_FRAME);

  EXPECT_THAT(doubles.inputs.sentActions(), ElementsAre(SHOOT_BIT));
}

/**
 * Given the game with AUTO-FIRE on
 * When the player presses up
 * Then up and shoot are held
 */
TEST(GameScreenAutoFireTest, KeepsTheMovesOfTheKeys) {
  LoneGameParts doubles;
  doubles.settings.autoFire = true;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());

  game.handleEvent(keyPress(sf::Keyboard::Key::Up, sf::Keyboard::Scan::Up));

  EXPECT_EQ(game.playerActions().bits(), UP_BIT | SHOOT_BIT);
}

/**
 * Given the game with AUTO-FIRE on and space pressed
 * When the player releases space
 * Then shoot is still held
 */
TEST(GameScreenAutoFireTest, ReleasingTheShootKeyKeepsShooting) {
  LoneGameParts doubles;
  doubles.settings.autoFire = true;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.handleEvent(
      keyPress(sf::Keyboard::Key::Space, sf::Keyboard::Scan::Space));

  game.handleEvent(
      keyRelease(sf::Keyboard::Key::Space, sf::Keyboard::Scan::Space));

  EXPECT_EQ(game.playerActions().bits(), SHOOT_BIT);
}

/**
 * Given the game with AUTO-FIRE on, and the options just opened over it
 * When the game is updated behind them
 * Then the input it sends holds shoot
 */
TEST(GameScreenAutoFireTest, ShootsBehindTheOptions) {
  LoneGameParts doubles;
  doubles.settings.autoFire = true;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.handleEvent(keyRelease(OPTIONS_KEY, sf::Keyboard::Scan::Escape));

  game.update(ONE_FRAME);

  EXPECT_THAT(doubles.inputs.sentActions(), ElementsAre(SHOOT_BIT));
}

/**
 * Given the game with AUTO-FIRE on, after the window lost the focus
 * When its first frame is updated
 * Then the input sent holds shoot
 */
TEST(GameScreenAutoFireTest, ShootsWithoutTheFocus) {
  LoneGameParts doubles;
  doubles.settings.autoFire = true;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.handleEvent(sf::Event{sf::Event::FocusLost{}});

  game.update(ONE_FRAME);

  EXPECT_THAT(doubles.inputs.sentActions(), ElementsAre(SHOOT_BIT));
}

/**
 * Given the game with AUTO-FIRE off, after a first input that held nothing
 * When AUTO-FIRE is turned on and the next input is due
 * Then that input holds shoot
 */
TEST(GameScreenAutoFireTest, TurningItOnDuringTheGameShootsAtTheNextInput) {
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.update(ONE_FRAME);

  doubles.settings.autoFire = true;
  game.update(PLAYER_INPUT_INTERVAL);

  EXPECT_THAT(doubles.inputs.sentActions(),
              ElementsAre(NOTHING_HELD, SHOOT_BIT));
}

/**
 * Given the game with AUTO-FIRE on, after a first input that held shoot
 * When AUTO-FIRE is turned off and the next input is due
 * Then that input holds nothing
 */
TEST(GameScreenAutoFireTest, TurningItOffDuringTheGameStopsAtTheNextInput) {
  LoneGameParts doubles;
  doubles.settings.autoFire = true;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  game.update(ONE_FRAME);

  doubles.settings.autoFire = false;
  game.update(PLAYER_INPUT_INTERVAL);

  EXPECT_THAT(doubles.inputs.sentActions(),
              ElementsAre(SHOOT_BIT, NOTHING_HELD));
}

/**
 * Given a game just started
 * When 117 ms pass
 * Then the stars of the sky go 12 times their speed
 */
TEST_F(GameScreenFixture, TheStarsWarpAtTheStartOfTheGame) {
  screens().update(WARP_TOP_TIME);

  EXPECT_NEAR(sky().lastSpeedFactor(), TOP_WARP_SPEED, WARP_FACTOR_TOLERANCE);
}

/**
 * Given a game just started
 * When 600 ms pass
 * Then the stars go their own speed again
 */
TEST_F(GameScreenFixture, TheStarsSlowDownToTheirSpeed) {
  screens().update(WARP_END_TIME);

  EXPECT_NEAR(sky().lastSpeedFactor(), OWN_SPEED, WARP_FACTOR_TOLERANCE);
}

/**
 * Given a player who reduced flashes
 * When the game starts and 117 ms pass
 * Then the stars keep their own speed
 */
TEST_F(GameScreenFixture, WithFlashesReducedTheStarsDoNotWarp) {
  settings().reduceFlashes = true;

  screens().update(WARP_TOP_TIME);

  EXPECT_EQ(sky().lastSpeedFactor(), OWN_SPEED);
}

/**
 * Given a game whose stars warp
 * When the game leaves the stack
 * Then the sky goes its own speed, in its place
 */
TEST_F(GameScreenFixture, LeavingTheGamePutsTheSkyBack) {
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
TEST_F(GameScreenFixture, TheBannerStartsAtOneSecond) {
  screens().update(BANNER_START_TIME);

  EXPECT_NE(writingOf(statusCanvas(), "M"), nullptr);
}

/**
 * Given a game at rest, no trauma
 * When a frame passes
 * Then the entity layer is painted in its place
 */
TEST_F(GameScreenFixture, TheEntitiesArePaintedInPlaceAtRest) {
  screens().update(ONE_FRAME);

  EXPECT_EQ(layerRecord().offsetAtLastPaint, sf::Vector2i());
}

/**
 * Given a game screen
 * When the server announces a missile of a player and the screen updates
 * Then the shot of a player plays through the sounds of its parts
 */
TEST_F(GameScreenFixture, TheGamePlaysTheSoundsOfWhatTheServerAnnounces) {
  entityMessages().queue(spawned(PLAYER_MISSILE_ID,
                                 WireEntityType::PlayerMissile,
                                 PLAYER_MISSILE_POSITION));

  screens().update(ONE_FRAME);

  EXPECT_THAT(sounds().played(), ElementsAre(PLAYER_SHOT_SOUND_ID));
}

/**
 * Given the game of three pilots
 * When the server ends it with their scores
 * Then the result ranks them best first, with their nicknames, NOVA as this
 * client
 */
TEST(GameScreenEndTest, TheEndOfTheGameRanksThePilotsOfTheBoard) {
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  doubles.entityMessages.queue(endOfThreePilots());

  game.update(ONE_FRAME);

  ASSERT_EQ(doubles.result.pilots.size(), PILOTS_AT_THE_END);
  EXPECT_EQ(doubles.result.pilots.front().nickname, SECOND_PILOT_NICKNAME);
  EXPECT_TRUE(doubles.result.pilots.front().isLocalPlayer);
  EXPECT_EQ(doubles.result.pilots.back().nickname, FIRST_PILOT_NICKNAME);
}

/**
 * Given a game the server just ended
 * When a second less one nanosecond passes
 * Then the end screen is not asked for yet
 */
TEST(GameScreenEndTest, TheEndScreenWaitsForTheLastExplosion) {
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  doubles.entityMessages.queue(endOfThreePilots());
  game.update(ONE_FRAME);

  game.update(END_WAIT - ONE_TICK);

  EXPECT_THAT(doubles.navigator.calls(), IsEmpty());
}

/**
 * Given a game the server just ended
 * When a second passes, then another
 * Then the end screen is asked for in place of every screen, once
 */
TEST(GameScreenEndTest, TheEndScreenReplacesTheGameASecondLater) {
  LoneGameParts doubles;
  GameScreen game(doubles.parts(), std::make_unique<IgnoringEntityLayer>(),
                  doubles.statusCanvasDouble());
  doubles.entityMessages.queue(endOfThreePilots());
  game.update(ONE_FRAME);

  game.update(END_WAIT);
  game.update(END_WAIT);

  EXPECT_THAT(doubles.navigator.calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::End}));
}

/**
 * Given a game where KAI is player 3
 * When the player picks deuteranopia in the options and a frame goes by
 * Then the status band is painted again, KAI in the light bluish green of the
 * palette
 */
TEST_F(GameScreenFixture, ChangingTheColorModeRepaintsTheStatusBand) {
  settings().colorMode = ColorMode::Deuteranopia;

  screens().update(ONE_FRAME);

  const RecordedWriting* name = writingOf(statusCanvas(), THIRD_PILOT_NICKNAME);
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->style.color, LIGHT_BLUISH_GREEN);
}

/**
 * Given a game of three pilots
 * When the server says the third one left
 * Then the feed writes LEFT THE GAME
 */
TEST_F(GameScreenFixture, APilotWhoLeftGetsALineInTheFeed) {
  entityMessages().queue(THIRD_PILOT_LEFT);

  screens().update(ONE_FRAME);

  EXPECT_NE(writingOf(statusCanvas(), FEED_LEFT_WORDS), nullptr);
}

/**
 * Given a game of three pilots
 * When the three copies of a PlayerLeft of the third one arrive
 * Then the feed writes one line
 */
TEST_F(GameScreenFixture, ThreeCopiesOfAPlayerLeftWriteOneLine) {
  for (int copy = 0; copy < PLAYER_LEFT_COPIES; ++copy) {
    entityMessages().queue(THIRD_PILOT_LEFT);
  }

  screens().update(ONE_FRAME);

  EXPECT_EQ(
      std::ranges::count(statusCanvas().writings, std::string{FEED_LEFT_WORDS},
                         &RecordedWriting::text),
      1);
}

/**
 * Given a line in the feed
 * When five seconds pass
 * Then the line is gone
 */
TEST_F(GameScreenFixture, TheLineGoesAfterItsTime) {
  entityMessages().queue(THIRD_PILOT_LEFT);
  screens().update(ONE_FRAME);

  screens().update(LINE_TIME);

  EXPECT_EQ(writingOf(statusCanvas(), FEED_LEFT_WORDS), nullptr);
}

/**
 * Given a game whose server sent nothing
 * When one second passes
 * Then WAITING FOR THE SERVER shows
 */
TEST_F(GameScreenFixture, ASecondWithoutMessagesShowsTheWarning) {
  screens().update(ONE_SECOND);

  EXPECT_NE(writingOf(statusCanvas(), WARNING_WORDS), nullptr);
}

/**
 * Given the warning shown
 * When a message of the server arrives
 * Then the warning is gone
 */
TEST_F(GameScreenFixture, AMessageOfTheServerHidesTheWarning) {
  screens().update(ONE_SECOND);
  entityMessages().queue(rtype::network::WorldState{});

  screens().update(ONE_FRAME);

  EXPECT_EQ(writingOf(statusCanvas(), WARNING_WORDS), nullptr);
}

/**
 * Given a game the server ended
 * When one second passes without a message
 * Then no warning shows: the server is quiet on purpose
 */
TEST_F(GameScreenFixture, NoWarningOnceTheGameEnded) {
  entityMessages().queue(rtype::network::GameEnded{});
  screens().update(ONE_FRAME);

  screens().update(ONE_SECOND);

  EXPECT_EQ(writingOf(statusCanvas(), WARNING_WORDS), nullptr);
}
