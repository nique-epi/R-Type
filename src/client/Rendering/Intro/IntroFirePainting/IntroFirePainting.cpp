#include "IntroFirePainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <span>
#include "Animation.hpp"
#include "CinemaProjection.hpp"
#include "Easing.hpp"
#include "EffectAnimations.hpp"
#include "EffectColorConstants.hpp"
#include "EffectConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "IntroBlast.hpp"
#include "IntroBurst.hpp"
#include "IntroCastConstants.hpp"
#include "IntroFrame.hpp"
#include "IntroMissile.hpp"
#include "IntroPaintConstants.hpp"
#include "IntroRing.hpp"
#include "IntroSpark.hpp"
#include "IntroTimeline.hpp"
#include "RenderingConstants.hpp"
#include "SpriteFrame.hpp"
#include "TextureArea.hpp"
#include "WorldArea.hpp"
#include "WorldImage.hpp"

namespace rtype::client {

namespace {

/** @brief The sparks or the drops of a burst at an age, and the colors they
 * fade through. */
struct DotSpray {
  sf::Vector2f origin;
  float age{};
  std::span<const IntroSpark> dots;
  std::span<const sf::Color> ramp;
};

/** @brief A circle of a ring: its center, its radius and its color, and the
 * step of the midpoint algorithm being painted. */
struct Circle {
  sf::Vector2f center;
  int radius{};
  sf::Color color;
  sf::Vector2i step;
};

/** @returns The version of an effect the preference asks for. */
const Animation& versionOf(const EffectAnimations& effect,
                           FlashPreference preference) {
  return preference == FlashPreference::Full ? effect.standard
                                             : effect.withoutFlash;
}

/** @returns The frame of the effect sheet an animation shows @p age seconds
 * after it started. */
TextureArea effectImageAt(const Animation& animation, float age) {
  const game::SpriteFrame frame =
      animation.frameAt(durationOf(age)).value_or(animation.frames.front());
  return TextureArea{.assetId = EFFECT_SHEET_ID,
                     .rectangle = rectangleOf(frame)};
}

/** @returns Whether an animation started @p age seconds ago still shows. */
bool stillShows(const Animation& animation, float age) {
  return age >= 0.0F && !animation.hasEnded(durationOf(age));
}

/** @returns Where a missile is, @p flown of the way through its flight: on
 * its line, or off it by its swerve. */
sf::Vector2f pointAlong(const IntroMissile& missile, float flown) {
  const sf::Vector2f line = missile.to - missile.from;
  const sf::Vector2f straight = missile.from + (line * flown);
  if (missile.curve == 0.0F) {
    return straight;
  }
  const float length = line.length() > 0.0F ? line.length() : 1.0F;
  const float swerve =
      std::sin(std::numbers::pi_v<float> * flown) *
      (missile.curve +
       (INTRO_SWERVE_WOBBLE *
        std::sin((flown * INTRO_SWERVE_WAVES) + missile.curve)));
  return straight + (sf::Vector2f(-line.y, line.x) * (swerve / length));
}

/** @returns How far a missile is through its flight, from 0 to 1; outside
 * of it before it leaves and once it hit. */
float flightOf(const IntroMissile& missile, float scene) {
  return (scene - missile.start) / (missile.end - missile.start);
}

/** @brief Paints the smoke behind a swerving missile. */
void paintSmoke(IInterfaceCanvas& canvas, const IntroFrame& frame,
                const IntroMissile& missile) {
  const float flown = flightOf(missile, frame.scene);
  for (std::size_t puff = INTRO_SMOKE_RAMP.size(); puff > 0; --puff) {
    const float back = flown - (static_cast<float>(puff) * INTRO_SMOKE_SPACING);
    if (back <= 0.0F) {
      continue;
    }
    const int size =
        puff > INTRO_LARGE_PUFFS ? INTRO_SMALL_PUFF : INTRO_LARGE_PUFF;
    paintWorldArea(canvas, frame,
                   WorldArea{.topLeft = pointAlong(missile, back),
                             .size = {size, size},
                             .color = INTRO_SMOKE_RAMP.at(puff - 1)});
  }
}

/** @brief Paints the head of a missile of a player, centered where it
 * flies. */
void paintMissileHead(IInterfaceCanvas& canvas, const IntroFrame& frame,
                      const IntroMissile& missile) {
  const game::SpriteFrame head =
      PLAYER_MISSILE_ANIMATION.frameAt(durationOf(frame.scene))
          .value_or(PLAYER_MISSILE_FRAME);
  drawWorldImage(
      canvas, frame,
      WorldImage{
          .image = TextureArea{.assetId = ENTITY_SHEET_ID,
                               .rectangle = rectangleOf(head)},
          .topLeft = pointAlong(missile, flightOf(missile, frame.scene)) -
                     INTRO_PLAYER_MISSILE_HALF});
}

/** @brief Paints the muzzle flash of a missile of a player during
 * INTRO_MUZZLE_TIME after its shot. */
void paintMuzzle(IInterfaceCanvas& canvas, const IntroFrame& frame,
                 const IntroMissile& missile) {
  const float age = frame.scene - missile.start;
  if (age >= INTRO_MUZZLE_TIME) {
    return;
  }
  drawWorldImage(
      canvas, frame,
      WorldImage{
          .image = effectImageAt(
              versionOf(PLAYER_MUZZLE_FLASH, frame.preference), age),
          .topLeft = missile.from - sf::Vector2f(0.0F, INTRO_MUZZLE_RISE)});
}

/** @brief Paints a missile of a player in flight: its smoke when it swerves,
 * its head, and its muzzle flash just after the shot. */
void paintFlyingMissile(IInterfaceCanvas& canvas, const IntroFrame& frame,
                        const IntroMissile& missile) {
  if (missile.curve != 0.0F) {
    paintSmoke(canvas, frame, missile);
  }
  paintMissileHead(canvas, frame, missile);
  paintMuzzle(canvas, frame, missile);
}

/** @brief Paints the impact of a missile that hit, while it shows. */
void paintImpact(IInterfaceCanvas& canvas, const IntroFrame& frame,
                 const IntroMissile& missile) {
  const Animation& impact = versionOf(IMPACT, frame.preference);
  const float age = frame.scene - missile.end;
  if (!stillShows(impact, age)) {
    return;
  }
  drawWorldImage(canvas, frame,
                 WorldImage{.image = effectImageAt(impact, age),
                            .topLeft = missile.to - INTRO_IMPACT_HALF,
                            .leftBehind = true});
}

/** @brief Paints the explosions of the sheet of a burst, each from its
 * delay. */
void paintBlasts(IInterfaceCanvas& canvas, const IntroFrame& frame,
                 const IntroBurst& burst) {
  for (const IntroBlast& blast : burst.blasts) {
    const Animation& animation = versionOf(
        blast.fireball ? SHIP_EXPLOSION : BYDO_EXPLOSION, frame.preference);
    const float age = frame.scene - burst.start - blast.delay;
    if (!stillShows(animation, age)) {
      continue;
    }
    const sf::Vector2f half =
        blast.fireball ? INTRO_FIREBALL_HALF : INTRO_BYDO_BURST_HALF;
    drawWorldImage(canvas, frame,
                   WorldImage{.image = effectImageAt(animation, age),
                              .topLeft = burst.center + blast.offset - half,
                              .leftBehind = true});
  }
}

/** @brief Paints the sparks or the drops of a burst that are alive, each
 * slowing down and fading through its ramp. */
void paintDots(IInterfaceCanvas& canvas, const IntroFrame& frame,
               const DotSpray& spray) {
  for (const IntroSpark& dot : spray.dots) {
    const float flown = spray.age - dot.delay;
    if (flown < 0.0F || flown > dot.life) {
      continue;
    }
    const float gone = flown / dot.life;
    const auto shade = std::min(
        spray.ramp.size() - 1,
        static_cast<std::size_t>(gone * static_cast<float>(spray.ramp.size())));
    paintWorldArea(
        canvas, frame,
        WorldArea{.topLeft = spray.origin +
                             (dot.velocity * (flown * (1.0F - (gone * HALF)))),
                  .size = {dot.size, dot.size},
                  .color = spray.ramp[shade],
                  .leftBehind = true});
  }
}

/** @brief Paints the eight points of a circle that one step of the midpoint
 * algorithm finds, @p step from its center. */
void paintOctants(IInterfaceCanvas& canvas, const IntroFrame& frame,
                  const Circle& circle) {
  const sf::Vector2i step = circle.step;
  for (const sf::Vector2i point :
       {sf::Vector2i(step.x, step.y), sf::Vector2i(step.y, step.x),
        sf::Vector2i(-step.y, step.x), sf::Vector2i(-step.x, step.y),
        sf::Vector2i(-step.x, -step.y), sf::Vector2i(-step.y, -step.x),
        sf::Vector2i(step.y, -step.x), sf::Vector2i(step.x, -step.y)}) {
    paintWorldArea(canvas, frame,
                   WorldArea{.topLeft = circle.center + sf::Vector2f(point),
                             .size = {1, 1},
                             .color = circle.color,
                             .leftBehind = true});
  }
}

/** @brief Paints a circle one pixel wide, after the midpoint algorithm: one
 * step per row of its first octant. */
void paintCircle(IInterfaceCanvas& canvas, const IntroFrame& frame,
                 Circle circle) {
  int column = circle.radius;
  int row = 0;
  int error = 1 - column;
  while (column >= row) {
    circle.step = {column, row};
    paintOctants(canvas, frame, circle);
    ++row;
    if (error < 0) {
      error += row + row + 1;
    } else {
      --column;
      error += (row - column) + (row - column) + 1;
    }
  }
}

}  // namespace

void paintEnemyMissiles(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  const game::SpriteFrame sprite =
      ENEMY_MISSILE_ANIMATION.frameAt(durationOf(frame.scene))
          .value_or(ENEMY_MISSILE_FRAME);
  for (const IntroMissile& missile : frame.cast.get().missiles) {
    const float flown = flightOf(missile, frame.scene);
    if (!missile.enemy || flown < 0.0F || flown >= 1.0F) {
      continue;
    }
    drawWorldImage(
        canvas, frame,
        WorldImage{
            .image = TextureArea{.assetId = ENTITY_SHEET_ID,
                                 .rectangle = rectangleOf(sprite)},
            .topLeft = pointAlong(missile, flown) - INTRO_ENEMY_MISSILE_HALF,
            .leftBehind = true});
  }
}

void paintPlayerMissiles(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  for (const IntroMissile& missile : frame.cast.get().missiles) {
    if (missile.enemy) {
      continue;
    }
    const float flown = flightOf(missile, frame.scene);
    if (flown >= 0.0F && flown < 1.0F) {
      paintFlyingMissile(canvas, frame, missile);
    }
    paintImpact(canvas, frame, missile);
  }
}

void paintBursts(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  for (const IntroBurst& burst : frame.cast.get().bursts) {
    const float age = frame.scene - burst.start;
    if (age < 0.0F || age > INTRO_BURST_LIFE) {
      continue;
    }
    paintBlasts(canvas, frame, burst);
    paintDots(canvas, frame,
              DotSpray{.origin = burst.center,
                       .age = age,
                       .dots = burst.sparks,
                       .ramp = std::span(SPARK_RAMP).subspan(1)});
    paintDots(canvas, frame,
              DotSpray{.origin = burst.center,
                       .age = age,
                       .dots = burst.drops,
                       .ramp = BYDO_RAMP});
  }
}

void paintRings(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  for (const IntroRing& ring : frame.cast.get().rings) {
    const float through = (frame.scene - ring.start) / ring.life;
    if (through < 0.0F || through >= 1.0F) {
      continue;
    }
    const int radius = std::max(
        1, static_cast<int>(std::round(ring.radius * easeOutCubic(through))));
    const auto shade =
        std::min(BIG_CATCH_RING_RAMP.size() - 1,
                 static_cast<std::size_t>(
                     through * static_cast<float>(BIG_CATCH_RING_RAMP.size())));
    paintCircle(canvas, frame,
                Circle{.center = ring.center,
                       .radius = radius,
                       .color = BIG_CATCH_RING_RAMP.at(shade),
                       .step = {}});
  }
}

}  // namespace rtype::client
