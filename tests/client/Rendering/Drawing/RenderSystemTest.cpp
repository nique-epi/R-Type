#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "Animation.hpp"
#include "Caption.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Effects/EffectTestConstants.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "FrameAnchor.hpp"
#include "HitFlash.hpp"
#include "Invulnerability.hpp"
#include "Layer.hpp"
#include "Nudge.hpp"
#include "PilotTestConstants.hpp"
#include "Playback.hpp"
#include "Position.hpp"
#include "RecordingDrawSurface.hpp"
#include "RenderSystemFixtures.hpp"
#include "RenderSystemTestConstants.hpp"
#include "RenderingConstants.hpp"
#include "Reveal.hpp"
#include "Scene.hpp"
#include "ShipLook.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "SteadyMotion.hpp"
#include "TimeConstants.hpp"
#include "WornLook.hpp"

using rtype::client::Animation;
using rtype::client::Caption;
using rtype::client::FrameAnchor;
using rtype::client::HALF;
using rtype::client::HitFlash;
using rtype::client::Invulnerability;
using rtype::client::Nudge;
using rtype::client::Playback;
using rtype::client::Reveal;
using rtype::client::SpriteAnimation;
using rtype::client::SpriteTone;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::CollisionBox;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;
using rtype::game::SteadyMotion;

/**
 * Given one entity in each of the seven layers, created from the front layer
 * to the back layer
 * When a frame is rendered
 * Then the images are drawn from the background to the interface: an
 * explosion covers a Bydo, never a ship nor a missile of an enemy
 */
TEST(RenderSystem, DrawsLayersFromBackgroundToInterface) {
  Scene scene;
  scene.spawn("score", Layer::Interface);
  scene.spawn("enemy-missile", Layer::EnemyMissiles);
  scene.spawn("figure-plate", Layer::Popups);
  scene.spawn("ship", Layer::Players);
  scene.spawn("explosion", Layer::Effects);
  scene.spawn("bydo", Layer::Enemies);
  scene.spawn("background", Layer::Background);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(), allLayersInOrder());
}

/**
 * Given two entities in the same layer
 * When a frame is rendered
 * Then the one with the lower entity index is drawn first
 */
TEST(RenderSystem, DrawsEntitiesOfOneLayerByAscendingIndex) {
  Scene scene;
  const Entity first = scene.spawn("first", Layer::Enemies);
  const Entity second = scene.spawn("second", Layer::Enemies);
  ASSERT_LT(first.index, second.index);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(),
            (std::vector<std::string>{"first", "second"}));
}

/**
 * Given an entity at a known position
 * When a frame is rendered
 * Then the image is drawn centered on that position
 */
TEST(RenderSystem, DrawsAtThePositionOfTheEntity) {
  Scene scene;
  const Entity entity = scene.spawn("ship", Layer::Enemies);
  scene.components().get<Position>(entity)->x = SHIP_X;
  scene.components().get<Position>(entity)->y = SHIP_Y;

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_FLOAT_EQ(scene.surface().drawn()[0].center.x, SHIP_X);
  EXPECT_FLOAT_EQ(scene.surface().drawn()[0].center.y, SHIP_Y);
}

/**
 * Given an entity with a Position and no Sprite, and an entity with a Sprite
 * and no Position
 * When a frame is rendered
 * Then neither is drawn
 */
TEST(RenderSystem, SkipsEntitiesMissingPositionOrSprite) {
  Scene scene;
  const Entity withoutSprite = scene.entities().create();
  scene.components().add(withoutSprite, Position{.x = 0.0F, .y = 0.0F});
  const Entity withoutPosition = scene.entities().create();
  scene.components().add(withoutPosition, Sprite{.assetId = "ghost",
                                                 .frame = WHOLE_IMAGE,
                                                 .layer = Layer::Enemies});

  scene.render();

  EXPECT_TRUE(scene.surface().drawn().empty());
}

/**
 * Given an entity that also carries gameplay components
 * When a frame is rendered
 * Then it is drawn exactly like an entity without them
 */
TEST(RenderSystem, GameplayComponentsDoNotChangeWhatIsDrawn) {
  Scene scene;
  const Entity armed = scene.spawn("armed", Layer::Enemies);
  scene.components().add(armed, SteadyMotion{.topSpeed = SPEED});
  scene.components().add(armed,
                         CollisionBox{.width = BOX_SIZE, .height = BOX_SIZE});
  scene.spawn("plain", Layer::Enemies);

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), 2U);
  EXPECT_FLOAT_EQ(drawn[0].center.x, drawn[1].center.x);
  EXPECT_FLOAT_EQ(drawn[0].center.y, drawn[1].center.y);
}

/**
 * Given an entity that was destroyed
 * When a frame is rendered
 * Then it is not drawn
 */
TEST(RenderSystem, DoesNotDrawDestroyedEntities) {
  Scene scene;
  const Entity gone = scene.spawn("gone", Layer::Enemies);
  scene.spawn("kept", Layer::Enemies);
  scene.components().destroy(gone);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(), (std::vector<std::string>{"kept"}));
}

/**
 * Given a registry holding no entity
 * When a frame is rendered
 * Then nothing is drawn
 */
TEST(RenderSystem, DrawsNothingForAnEmptyRegistry) {
  Scene scene;

  scene.render();

  EXPECT_TRUE(scene.surface().drawn().empty());
}

/**
 * Given a system that already rendered a frame
 * When a second frame is rendered with the same entities
 * Then the second frame draws the same sequence, with nothing left over from
 * the first
 */
TEST(RenderSystem, EachFrameDrawsTheSameSequence) {
  Scene scene;
  scene.spawn("ship", Layer::Enemies);
  scene.spawn("background", Layer::Background);
  scene.render();
  const std::vector<std::string> firstFrame = scene.surface().assetIds();
  scene.surface().forget();

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(), firstFrame);
}

/**
 * Given two entities of one layer whose components were added from the higher
 * index to the lower
 * When a frame is rendered
 * Then the one with the lower entity index is still drawn first
 */
TEST(RenderSystem, DrawingOrderDoesNotFollowTheOrderComponentsWereAdded) {
  Scene scene;
  const Entity first = scene.entities().create();
  const Entity second = scene.entities().create();
  ASSERT_LT(first.index, second.index);
  scene.addPositionAndSprite(second, "second", Layer::Enemies);
  scene.addPositionAndSprite(first, "first", Layer::Enemies);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(),
            (std::vector<std::string>{"first", "second"}));
}

/**
 * Given an entity whose sprite shows one rectangle of its image
 * When a frame is rendered
 * Then the surface is asked to show that rectangle
 */
TEST(RenderSystem, ShowsTheRectangleTheSpriteNames) {
  Scene scene;
  const Entity entity = scene.spawn("sheet", Layer::Enemies);
  scene.components().get<Sprite>(entity)->frame = SHEET_CELL;

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn()[0].frame, SHEET_CELL);
}

/**
 * Given an entity whose layer is past the last drawing layer, and an entity in
 * the entities layer
 * When a frame is rendered
 * Then only the entity of a drawing layer is drawn
 */
TEST(RenderSystem, SkipsASpriteWhoseLayerIsNotADrawingLayer) {
  Scene scene;
  scene.spawn("lost", FIRST_VALUE_AFTER_THE_LAST_LAYER);
  scene.spawn("kept", Layer::Enemies);

  scene.render();

  EXPECT_EQ(scene.surface().assetIds(), (std::vector<std::string>{"kept"}));
}

/**
 * Given an entity that carries no hit flash
 * When a frame is rendered
 * Then it is drawn in the colors of its sheet
 */
TEST(RenderSystem, AnEntityWithoutHitFlashIsDrawnInItsOwnColors) {
  Scene scene;
  scene.spawn("ship", Layer::Enemies);

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().tone, SpriteTone::Normal);
}

/**
 * Given an entity flashed white
 * When a frame is rendered
 * Then it is drawn with the flash tone
 */
TEST(RenderSystem, AFlashedEntityIsDrawnWithTheFlashTone) {
  Scene scene;
  const Entity hit = scene.spawn("bydo", Layer::Enemies);
  scene.components().add(
      hit, HitFlash{.tone = SpriteTone::Flash, .remaining = HIT_FLASH_LENGTH});

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().tone, SpriteTone::Flash);
}

/**
 * Given an entity shaded, as a hit when the player reduced flashes
 * When a frame is rendered
 * Then it is drawn with the shade tone, the one its hit flash carries
 */
TEST(RenderSystem, AShadedEntityIsDrawnWithTheShadeTone) {
  Scene scene;
  const Entity hit = scene.spawn("bydo", Layer::Enemies);
  scene.components().add(
      hit, HitFlash{.tone = SpriteTone::Shade, .remaining = HIT_FLASH_LENGTH});

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().tone, SpriteTone::Shade);
}

/**
 * Given a figure of a gain in the Popups layer, a ship and a missile of an
 * enemy
 * When a frame is rendered
 * Then the figure is written, and the ship and the missile are drawn around
 * it: the ship before, the missile after
 */
TEST(RenderSystem, WritesACaptionInItsLayer) {
  Scene scene;
  scene.spawn("enemy-missile", Layer::EnemyMissiles);
  scene.spawn("ship", Layer::Players);
  const Entity figure = scene.entities().create();
  scene.components().add(figure, Position{.x = SHIP_X, .y = SHIP_Y});
  scene.components().add(figure, Caption{.text = "+100",
                                         .style = {},
                                         .layer = Layer::Popups,
                                         .visible = true});

  scene.render();

  ASSERT_EQ(scene.surface().written().size(), 1U);
  EXPECT_EQ(scene.surface().written().front().text, "+100");
  EXPECT_EQ(scene.surface().written().front().center.x, SHIP_X);
  EXPECT_EQ(scene.surface().assetIds(),
            (std::vector<std::string>{"ship", "enemy-missile"}));
}

/**
 * Given a caption on an entity, moved 12 units right and 56 units up
 * When a frame is rendered
 * Then its line is centered at the position of the entity moved by that offset
 */
TEST(RenderSystem, ACaptionIsWrittenAtItsOffset) {
  Scene scene;
  const Entity ship = scene.entities().create();
  scene.components().add(ship, Position{.x = SHIP_X, .y = SHIP_Y});
  scene.components().add(
      ship, Caption{.text = "ACE",
                    .style = {},
                    .layer = Layer::Popups,
                    .visible = true,
                    .offset = {.x = CAPTION_OFFSET_X, .y = CAPTION_OFFSET_Y}});

  scene.render();

  ASSERT_EQ(scene.surface().written().size(), 1U);
  EXPECT_EQ(scene.surface().written().front().center.x,
            SHIP_X + CAPTION_OFFSET_X);
  EXPECT_EQ(scene.surface().written().front().center.y,
            SHIP_Y + CAPTION_OFFSET_Y);
}

/**
 * Given a figure hidden between two blinks
 * When a frame is rendered
 * Then nothing is written
 */
TEST(RenderSystem, AHiddenCaptionIsNotWritten) {
  Scene scene;
  const Entity figure = scene.entities().create();
  scene.components().add(figure, Position{.x = SHIP_X, .y = SHIP_Y});
  scene.components().add(figure, Caption{.text = "+100",
                                         .style = {},
                                         .layer = Layer::Popups,
                                         .visible = false});

  scene.render();

  EXPECT_TRUE(scene.surface().written().empty());
}

/**
 * Given a Bydo nudged one pixel right
 * When a frame is rendered
 * Then it is drawn 4 units right of its position, one pixel of the canvas
 */
TEST(RenderSystem, ANudgedEntityIsDrawnAPixelAway) {
  Scene scene;
  const Entity bydo = scene.spawn("bydo", Layer::Enemies);
  scene.components().add(bydo, Nudge{.amount = ONE_PIXEL_RIGHT,
                                     .flipEvery = Duration::zero(),
                                     .elapsed = Duration::zero(),
                                     .duration = NUDGE_TIME});

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().center.x,
            SPAWN_X + UNITS_PER_PIXEL);
  EXPECT_EQ(scene.surface().drawn().front().center.y, SPAWN_Y);
}

/**
 * Given a Bydo whose animation hangs its frame 3 pixels left and 2 pixels down
 * When a frame is rendered
 * Then it is drawn 3 pixels of the canvas left and 2 down of its position
 */
TEST(RenderSystem, AnAnimationAnchorMovesTheFrameItDraws) {
  Scene scene;
  const Entity bydo = scene.spawn("bydo", Layer::Enemies);
  scene.components().add(
      bydo, SpriteAnimation{.animation = Animation{
                                .frames = {},
                                .frameDuration = Duration::zero(),
                                .playback = Playback::Hold,
                                .anchor = FrameAnchor{.x = -3, .y = 2}}});

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().center.x,
            SPAWN_X - (3 * UNITS_PER_PIXEL));
  EXPECT_EQ(scene.surface().drawn().front().center.y,
            SPAWN_Y + (2 * UNITS_PER_PIXEL));
}

/**
 * Given a victim vibrating, one frame per side
 * When it is rendered on its first step, then on its second
 * Then it is drawn one pixel right, then one pixel left
 */
TEST(RenderSystem, AVibratingEntityChangesSideEveryStep) {
  Scene scene;
  const Entity victim = scene.spawn("bydo", Layer::Enemies);
  scene.components().add(victim, Nudge{.amount = ONE_PIXEL_RIGHT,
                                       .flipEvery = VIBRATION_STEP_TIME,
                                       .elapsed = Duration::zero(),
                                       .duration = NUDGE_TIME});
  scene.render();
  const float firstX = scene.surface().drawn().back().center.x;
  scene.components().get<Nudge>(victim)->elapsed = VIBRATION_STEP_TIME;

  scene.render();

  EXPECT_EQ(firstX, SPAWN_X + UNITS_PER_PIXEL);
  EXPECT_EQ(scene.surface().drawn().back().center.x, SPAWN_X - UNITS_PER_PIXEL);
}

/**
 * Given an invulnerable ship blinking every 67 ms
 * When it is rendered at 0 ms, then at 67 ms
 * Then it is drawn in its own colors, then dithered
 */
TEST(RenderSystem, AnInvulnerableShipBlinksBetweenItselfAndItsDither) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(
      ship, Invulnerability{.elapsed = Duration::zero(), .steady = false});
  scene.render();
  const SpriteTone first = scene.surface().drawn().back().tone;
  scene.components().get<Invulnerability>(ship)->elapsed = BLINK_STEP_TIME;

  scene.render();

  EXPECT_EQ(first, SpriteTone::Normal);
  EXPECT_EQ(scene.surface().drawn().back().tone, SpriteTone::Ghost);
}

/**
 * Given a steady invulnerable ship outlined in the light pink of the second
 * pilot, as a player who reduced flashes sees it
 * When a frame is rendered
 * Then it is dithered and outlined in that pink
 */
TEST(RenderSystem, ASteadyInvulnerableShipIsDitheredAndOutlined) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(
      ship, Invulnerability{.elapsed = Duration::zero(),
                            .steady = true,
                            .outline = PINK_LIGHT_OF_THE_SECOND_PILOT});

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().tone, SpriteTone::Ghost);
  EXPECT_EQ(scene.surface().drawn().front().outline,
            PINK_LIGHT_OF_THE_SECOND_PILOT);
}

/**
 * Given an invulnerable ship just hit
 * When a frame is rendered
 * Then it is drawn with the flash tone: the hit wins over the dither
 */
TEST(RenderSystem, AHitFlashWinsOverInvulnerability) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(
      ship, Invulnerability{.elapsed = Duration::zero(), .steady = true});
  scene.components().add(
      ship, HitFlash{.tone = SpriteTone::Flash, .remaining = HIT_FLASH_LENGTH});

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().tone, SpriteTone::Flash);
}

/**
 * Given a ship whose reveal is in its first half
 * When a frame is rendered
 * Then nothing of it is drawn
 */
TEST(RenderSystem, AShipArrivingIsHiddenOnTheFirstHalf) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, Reveal{.age = A_QUARTER_OF_A_REVEAL,
                                      .duration = A_REVEAL,
                                      .leadingLine = true});

  scene.render();

  EXPECT_TRUE(scene.surface().drawn().empty());
}

/**
 * Given a ship 16 rows tall at three quarters of its reveal
 * When a frame is rendered
 * Then its top 8 rows are drawn where they lie in the whole frame, then one
 * white row under them
 */
TEST(RenderSystem, AShipArrivingShowsItsTopRowsAndALeadingLine) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, Reveal{.age = THREE_QUARTERS_OF_A_REVEAL,
                                      .duration = A_REVEAL,
                                      .leadingLine = true});

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), 2U);
  EXPECT_EQ(drawn[0].frame, (SpriteFrame{.left = WHOLE_IMAGE.left,
                                         .top = WHOLE_IMAGE.top,
                                         .width = WHOLE_IMAGE.width,
                                         .height = HALF_OF_THE_ROWS}));
  EXPECT_EQ(drawn[0].center.y, SPAWN_Y - (static_cast<float>(HALF_OF_THE_ROWS) *
                                          UNITS_PER_PIXEL * HALF));
  EXPECT_EQ(drawn[1].tone, SpriteTone::Flash);
  EXPECT_EQ(drawn[1].frame.top, WHOLE_IMAGE.top + HALF_OF_THE_ROWS);
  EXPECT_EQ(drawn[1].frame.height, 1);
}

/**
 * Given a ship halfway through its roll, whose reveal is in its first half
 * When a frame is rendered
 * Then nothing of it is drawn: the arrival hides it, roll or not
 */
TEST(RenderSystem, ARollingShipArrivingIsHiddenOnTheFirstHalf) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, halfwayRollingFlight());
  scene.components().add(ship, Reveal{.age = A_QUARTER_OF_A_REVEAL,
                                      .duration = A_REVEAL,
                                      .leadingLine = true});

  scene.render();

  EXPECT_TRUE(scene.surface().drawn().empty());
}

/**
 * Given a ship halfway through its roll
 * When a frame is rendered
 * Then all 16 rows of its frame are drawn one by one, shaded: its belly
 * shows
 */
TEST(RenderSystem, ARollingShipIsDrawnRowByRowAndShadedOnItsBelly) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, halfwayRollingFlight());

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), static_cast<std::size_t>(WHOLE_IMAGE.height));
  for (const DrawnImage& row : drawn) {
    EXPECT_EQ(row.frame.height, 1);
    EXPECT_EQ(row.tone, SpriteTone::Shade);
  }
}

/**
 * Given a steady invulnerable ship halfway through its roll
 * When a frame is rendered
 * Then every row of its belly stays dithered: the roll does not hide that
 * the ship cannot be hit
 */
TEST(RenderSystem, AnInvulnerableRollingShipStaysDitheredOnItsBelly) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, halfwayRollingFlight());
  scene.components().add(
      ship, Invulnerability{.elapsed = Duration::zero(),
                            .steady = true,
                            .outline = PINK_LIGHT_OF_THE_SECOND_PILOT});

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_FALSE(drawn.empty());
  for (const DrawnImage& row : drawn) {
    EXPECT_EQ(row.tone, SpriteTone::Ghost);
  }
}

/**
 * Given a dashing ship whose trail is known
 * When a frame is rendered
 * Then AFTERIMAGE_COUNT dithered copies are drawn before the ship itself
 */
TEST(RenderSystem, ADashingShipLeavesItsAfterimages) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, dashingFlight());

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), AFTERIMAGE_COUNT + 1);
  std::vector<float> afterimageColumns;
  for (std::size_t index = 0; index < AFTERIMAGE_COUNT; ++index) {
    EXPECT_EQ(drawn[index].tone, SpriteTone::Ghost);
    afterimageColumns.push_back(drawn[index].center.x);
  }
  std::ranges::sort(afterimageColumns);
  for (std::size_t index = 0; index < AFTERIMAGE_COUNT; ++index) {
    EXPECT_FLOAT_EQ(
        afterimageColumns.at(index),
        SHIP_X + static_cast<float>(AFTERIMAGE_FRAMES_BEHIND.at(index)));
  }
  EXPECT_EQ(drawn.back().tone, SpriteTone::Normal);
}

/**
 * Given a dashing ship whose trail is known, at three quarters of its
 * reveal
 * When a frame is rendered
 * Then only its top rows and the leading line are drawn: no afterimage
 * follows a ship still arriving
 */
TEST(RenderSystem, ADashingShipArrivingLeavesNoAfterimage) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, dashingFlight());
  scene.components().add(ship, Reveal{.age = THREE_QUARTERS_OF_A_REVEAL,
                                      .duration = A_REVEAL,
                                      .leadingLine = true});

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), 2U);
  EXPECT_EQ(drawn[0].frame.height, HALF_OF_THE_ROWS);
  EXPECT_EQ(drawn[1].tone, SpriteTone::Flash);
}

/**
 * Given a steady invulnerable ship whose pilot is not known, so no outline was
 * chosen for it
 * When a frame is rendered
 * Then it is dithered and drawn without outline
 */
TEST(RenderSystem, ASteadyInvulnerabilityWithoutOutlineDrawsNone) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, Invulnerability{.elapsed = Duration::zero(),
                                               .steady = true,
                                               .outline = std::nullopt});

  scene.render();

  ASSERT_EQ(scene.surface().drawn().size(), 1U);
  EXPECT_EQ(scene.surface().drawn().front().tone, SpriteTone::Ghost);
  EXPECT_FALSE(scene.surface().drawn().front().outline.has_value());
}

namespace {

/** @brief The top hat on the head of a ship drawn with the whole 16 x 16 frame
 * of the scene at (SPAWN_X, SPAWN_Y): its drawing is 14 x 10 pixels at the
 * top-left of the first cell of the sheet; its top-left corner lies at 9
 * pixels right and 7 up of the corner of the ship, so its center lies
 * (9 + 14 / 2 - 16 / 2) = 8 pixels right and (-7 + 10 / 2 - 16 / 2) = 10
 * pixels up of the center of the ship, one pixel being 4 logical units: 32
 * right, 40 up. */
constexpr rtype::game::SpriteFrame TOP_HAT_FRAME{
    .left = 0, .top = 0, .width = 14, .height = 10};
constexpr float TOP_HAT_CENTER_X = SPAWN_X + 32.0F;
constexpr float TOP_HAT_CENTER_Y = SPAWN_Y - 40.0F;
constexpr const char* LOOK_SHEET = "sprites/looks.png";

/** @returns The look of a ship that wears the top hat. */
rtype::client::WornLook wearingTheTopHat() {
  return rtype::client::WornLook{.look =
                                     rtype::identifiers::ShipLook{.head = 1}};
}

}  // namespace

/**
 * Given a ship wearing the top hat
 * When a frame is rendered
 * Then the ship is drawn, then the top hat over it, cut from the sheet of the
 * looks and centered 8 pixels right and 10 up of the ship
 */
TEST(RenderSystem, AWornItemIsDrawnOverItsShip) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, wearingTheTopHat());

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), 2U);
  EXPECT_EQ(drawn[0].assetId, "ship");
  EXPECT_EQ(drawn[1].assetId, LOOK_SHEET);
  EXPECT_EQ(drawn[1].frame, TOP_HAT_FRAME);
  EXPECT_EQ(drawn[1].center.x, TOP_HAT_CENTER_X);
  EXPECT_EQ(drawn[1].center.y, TOP_HAT_CENTER_Y);
}

/**
 * Given two ships of the same layer, the first one, with the lower entity
 * index, wearing the top hat
 * When a frame is rendered
 * Then its top hat is drawn right after it and before the second ship
 */
TEST(RenderSystem, AWornItemStaysRightAfterItsShip) {
  Scene scene;
  const Entity first = scene.spawn("first-ship", Layer::Players);
  scene.spawn("second-ship", Layer::Players);
  scene.components().add(first, wearingTheTopHat());

  scene.render();

  EXPECT_EQ(
      scene.surface().assetIds(),
      (std::vector<std::string>{"first-ship", LOOK_SHEET, "second-ship"}));
}

/**
 * Given a steady invulnerable ship, outlined, wearing the top hat
 * When a frame is rendered
 * Then the top hat is dithered and outlined as the ship is
 */
TEST(RenderSystem, AWornItemTakesTheToneOfAnInvulnerableShip) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, wearingTheTopHat());
  scene.components().add(
      ship, Invulnerability{.elapsed = Duration::zero(),
                            .steady = true,
                            .outline = PINK_LIGHT_OF_THE_SECOND_PILOT});

  scene.render();

  const std::vector<DrawnImage>& drawn = scene.surface().drawn();
  ASSERT_EQ(drawn.size(), 2U);
  EXPECT_EQ(drawn[1].assetId, LOOK_SHEET);
  EXPECT_EQ(drawn[1].tone, SpriteTone::Ghost);
  EXPECT_EQ(drawn[1].outline, PINK_LIGHT_OF_THE_SECOND_PILOT);
}

/**
 * Given a ship halfway through its roll, wearing the top hat
 * When a frame is rendered
 * Then no top hat is drawn: only the rows of the ship
 */
TEST(RenderSystem, NoItemIsDrawnWhileTheShipRolls) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, wearingTheTopHat());
  scene.components().add(ship, halfwayRollingFlight());

  scene.render();

  const std::vector<std::string> ids = scene.surface().assetIds();
  EXPECT_EQ(std::ranges::count(ids, std::string{LOOK_SHEET}), 0);
  EXPECT_FALSE(ids.empty());
}

/**
 * Given a ship at three quarters of its arrival, wearing the top hat
 * When a frame is rendered
 * Then no top hat is drawn: only the rows of the ship it has revealed
 */
TEST(RenderSystem, NoItemIsDrawnWhileTheShipArrives) {
  Scene scene;
  const Entity ship = scene.spawn("ship", Layer::Players);
  scene.components().add(ship, wearingTheTopHat());
  scene.components().add(ship, Reveal{.age = THREE_QUARTERS_OF_A_REVEAL,
                                      .duration = A_REVEAL,
                                      .leadingLine = false});

  scene.render();

  const std::vector<std::string> ids = scene.surface().assetIds();
  EXPECT_EQ(std::ranges::count(ids, std::string{LOOK_SHEET}), 0);
  EXPECT_FALSE(ids.empty());
}
