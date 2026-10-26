#include "IntroFleetPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <optional>
#include "Animation.hpp"
#include "BackgroundColorConstants.hpp"
#include "CinemaProjection.hpp"
#include "EffectColorConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "IntroCastConstants.hpp"
#include "IntroExhaustRow.hpp"
#include "IntroFrame.hpp"
#include "IntroMob.hpp"
#include "IntroMobKind.hpp"
#include "IntroMobPaths.hpp"
#include "IntroPaintConstants.hpp"
#include "IntroShipPaths.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "PlayerColors.hpp"
#include "SpriteFrame.hpp"
#include "TextureArea.hpp"
#include "WorldArea.hpp"
#include "WorldImage.hpp"

namespace rtype::client {

namespace {

/** @brief A ship on screen: its pilot and the top-left corner of its
 * sprite. */
struct ShipOnScreen {
  std::size_t pilot{};
  sf::Vector2f place;
};

/** @brief A row of a flame: its pilot, the pixel right of its first pixel,
 * and its length in pixels. */
struct FlameRow {
  std::size_t pilot{};
  sf::Vector2f nozzle;
  int length{};
};

/** @brief Paints a Bydo, when there is one. */
void paintMob(IInterfaceCanvas& canvas, const IntroFrame& frame,
              const std::optional<IntroMob>& mob) {
  if (!mob.has_value()) {
    return;
  }
  const Animation& animation = mob->kind == IntroMobKind::BonusBydo
                                   ? BONUS_BYDO_ANIMATION
                                   : BYDO_ANIMATION;
  const game::SpriteFrame sprite =
      animation.frameAt(durationOf(frame.scene)).value_or(BYDO_FRAME);
  drawWorldImage(
      canvas, frame,
      WorldImage{.image = TextureArea{.assetId = ENTITY_SHEET_ID,
                                      .rectangle = rectangleOf(sprite)},
                 .topLeft = mob->center - INTRO_MOB_HALF,
                 .tint = mob->flashed ? BYDO_LILAC : sf::Color::White,
                 .leftBehind = true});
}

/** @returns The sprite of the ship of @p pilot at that moment. */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
TextureArea shipImageAt(std::size_t pilot, float scene) {
  const game::SpriteFrame frame =
      PLAYER_SHIP_ANIMATION.frameAt(durationOf(scene))
          .value_or(PLAYER_SHIP_FRAME);
  return TextureArea{.assetId = ENTITY_SHEET_ID,
                     .rectangle = rectangleOf(
                         inPlayerColorRow(frame, static_cast<int>(pilot)))};
}

/** @brief Paints the ghosts of a ship, the farthest first: the two far ones
 * in the dark shade of its pilot, the near one in its color. */
void paintSmear(IInterfaceCanvas& canvas, const IntroFrame& frame,
                const ShipOnScreen& ship) {
  const float smear = smearAt(ship.pilot, frame.scene);
  if (smear <= INTRO_SMEAR_THRESHOLD) {
    return;
  }
  for (std::size_t copy = INTRO_SMEAR_COPIES; copy > 0; --copy) {
    const float back = static_cast<float>(copy) * INTRO_SMEAR_SPACING * smear;
    drawWorldImage(
        canvas, frame,
        WorldImage{.image = shipImageAt(ship.pilot, frame.scene),
                   .topLeft = ship.place - sf::Vector2f(back, 0.0F),
                   .tint = copy == 1
                               ? PLAYER_COLORS.at(ship.pilot).ship
                               : INTRO_PLAYER_DARK_COLORS.at(ship.pilot)});
  }
}

/** @returns The first pixel of a flame of @p length pixels that takes shade
 * @p shade: pixel k takes shade ⌊k × INTRO_FLAME_SHADES / length⌋. */
int firstPixelOfShade(std::size_t shade, int length) {
  const auto shades = static_cast<int>(INTRO_FLAME_SHADES);
  return ((static_cast<int>(shade) * length) + shades - 1) / shades;
}

/** @brief Paints one row of a flame, one rectangle per shade, from white at
 * the nozzle to the dark shade of the pilot at its end. */
void paintFlameRow(IInterfaceCanvas& canvas, const IntroFrame& frame,
                   const FlameRow& flame) {
  const PlayerColors& colors = PLAYER_COLORS.at(flame.pilot);
  const std::array<sf::Color, INTRO_FLAME_SHADES> shades{
      STAR_WHITE, colors.light, colors.ship,
      INTRO_PLAYER_DARK_COLORS.at(flame.pilot)};
  for (std::size_t shade = 0; shade < shades.size(); ++shade) {
    const int first = firstPixelOfShade(shade, flame.length);
    const int end = firstPixelOfShade(shade + 1, flame.length);
    if (end > first) {
      paintWorldArea(
          canvas, frame,
          WorldArea{.topLeft = flame.nozzle -
                               sf::Vector2f(static_cast<float>(end), 0.0F),
                    .size = {end - first, 1},
                    .color = shades.at(shade)});
    }
  }
}

/** @brief Paints the flame behind a ship, its rows flickering one pixel. */
void paintExhaust(IInterfaceCanvas& canvas, const IntroFrame& frame,
                  const ShipOnScreen& ship) {
  const float amount = exhaustAt(frame.scene);
  if (amount <= INTRO_EXHAUST_THRESHOLD) {
    return;
  }
  const float flicker =
      isOddStepAt(frame.scene, INTRO_FLICKER_STEPS_PER_SECOND) ? 1.0F : 0.0F;
  const float length =
      std::round(INTRO_EXHAUST_BASE + (amount * INTRO_EXHAUST_GROWTH)) +
      flicker;
  for (const IntroExhaustRow& row : INTRO_EXHAUST_ROWS) {
    paintFlameRow(
        canvas, frame,
        FlameRow{.pilot = ship.pilot,
                 .nozzle = ship.place +
                           sf::Vector2f(0.0F, static_cast<float>(row.row)),
                 .length = std::max(
                     1, static_cast<int>(std::round(length * row.share)))});
  }
}

}  // namespace

void paintMobs(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  for (const std::optional<IntroMob>& mob : openingMobsAt(frame.scene)) {
    paintMob(canvas, frame, mob);
  }
  for (std::size_t bydo = 0; bydo < INTRO_HELIX_SIZE; ++bydo) {
    paintMob(canvas, frame, helixMobAt(bydo, frame.scene));
  }
  for (std::size_t newcomer = 0; newcomer < INTRO_RING_SIZE; ++newcomer) {
    paintMob(canvas, frame, newcomerMobAt(newcomer, frame.scene));
  }
}

void paintShips(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  for (std::size_t pilot = 0; pilot < INTRO_HOMES.size(); ++pilot) {
    const std::optional<sf::Vector2f> place = shipPlaceAt(pilot, frame.scene);
    if (!place.has_value()) {
      continue;
    }
    const ShipOnScreen ship{.pilot = pilot, .place = *place};
    paintSmear(canvas, frame, ship);
    paintExhaust(canvas, frame, ship);
    drawWorldImage(canvas, frame,
                   WorldImage{.image = shipImageAt(pilot, frame.scene),
                              .topLeft = ship.place});
  }
}

}  // namespace rtype::client
