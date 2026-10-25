#include "IntroBoostPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <optional>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "CinemaProjection.hpp"
#include "Easing.hpp"
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "IntroCastConstants.hpp"
#include "IntroFrame.hpp"
#include "IntroHash.hpp"
#include "IntroPaintConstants.hpp"
#include "IntroShipPaths.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "PlayerColors.hpp"
#include "RenderingConstants.hpp"
#include "WorldArea.hpp"

namespace rtype::client {

namespace {

/** @brief The nose of a ship that charges, and its pilot. */
struct ChargingNose {
  std::size_t pilot{};
  sf::Vector2f point;
};

/** @brief A disc of a charge: its center, its radius and its color. */
struct Disc {
  sf::Vector2f center;
  int radius{};
  sf::Color color;
};

/** @brief A beam of a ship at a moment: its pilot, the nose it leaves, and
 * how far through its life it is, from 0 to 1. */
struct Beam {
  std::size_t pilot{};
  sf::Vector2f nose;
  float through{};
};

/** @returns Grain @p grain of the charge at @p nose at that moment of the
 * scene, on a spiral that ends at the nose; nothing while it does not
 * fly. */
std::optional<WorldArea> grainAt(const ChargingNose& nose, std::size_t grain,
                                 float scene) {
  const float chargeTime = INTRO_SCENE_RELEASE - INTRO_SCENE_CHARGE;
  const float start =
      INTRO_SCENE_CHARGE +
      (static_cast<float>(grain) / static_cast<float>(INTRO_CHARGE_GRAINS) *
       chargeTime * INTRO_GRAIN_SPREAD);
  const float flight = (scene - start) / INTRO_GRAIN_FLIGHT;
  if (flight < 0.0F || flight >= 1.0F) {
    return std::nullopt;
  }
  const float angle = (introHash(static_cast<double>(
                           (nose.pilot * INTRO_GRAIN_SEED_STEP) + grain)) *
                       FULL_TURN) +
                      (flight * INTRO_GRAIN_TURN);
  const float reach =
      INTRO_GRAIN_REACH * std::pow(1.0F - flight, INTRO_GRAIN_CURVE);
  return WorldArea{
      .topLeft = nose.point +
                 sf::Vector2f(std::cos(angle) * reach, std::sin(angle) * reach),
      .size = {grain % INTRO_LARGE_GRAIN_PERIOD == 0 ? INTRO_LARGE_GRAIN_WIDTH
                                                     : INTRO_SMALL_GRAIN_WIDTH,
               1},
      .color = flight > INTRO_GRAIN_WHITE_AFTER
                   ? STAR_WHITE
                   : PLAYER_COLORS.at(nose.pilot).light};
}

/** @brief Paints the grains a charge draws in. */
void paintGrains(IInterfaceCanvas& canvas, const IntroFrame& frame,
                 const ChargingNose& nose) {
  for (std::size_t grain = 0; grain < INTRO_CHARGE_GRAINS; ++grain) {
    if (const std::optional<WorldArea> area =
            grainAt(nose, grain, frame.scene)) {
      paintWorldArea(canvas, frame, *area);
    }
  }
}

/** @brief Paints a disc, one rectangle per row. */
void paintDisc(IInterfaceCanvas& canvas, const IntroFrame& frame,
               const Disc& disc) {
  for (int row = -disc.radius; row <= disc.radius; ++row) {
    const int half = static_cast<int>(std::floor(std::sqrt(
        static_cast<float>((disc.radius * disc.radius) - (row * row)) + HALF)));
    paintWorldArea(canvas, frame,
                   WorldArea{.topLeft = disc.center +
                                        sf::Vector2f(static_cast<float>(-half),
                                                     static_cast<float>(row)),
                             .size = {half + half + 1, 1},
                             .color = disc.color});
  }
}

/** @brief Paints the ball of a charge: three discs, from the color of the
 * pilot to white in its middle, growing and flickering one pixel. */
void paintChargeBall(IInterfaceCanvas& canvas, const IntroFrame& frame,
                     const ChargingNose& nose) {
  const float grown = progressThrough(frame.scene, INTRO_SCENE_CHARGE,
                                      INTRO_SCENE_RELEASE - INTRO_SCENE_CHARGE);
  const int flicker =
      isOddStepAt(frame.scene, INTRO_FLICKER_STEPS_PER_SECOND) ? 1 : 0;
  const int radius = static_cast<int>(std::round(INTRO_BALL_RADIUS +
                                                 (INTRO_BALL_GROWTH * grown))) +
                     flicker;
  const PlayerColors& colors = PLAYER_COLORS.at(nose.pilot);
  const std::array<sf::Color, INTRO_BALL_LAYERS> layers{
      colors.ship, colors.light, STAR_WHITE};
  for (std::size_t layer = 0; layer < layers.size(); ++layer) {
    paintDisc(canvas, frame,
              Disc{.center = nose.point,
                   .radius = radius - static_cast<int>(layer),
                   .color = layers.at(layer)});
  }
}

/** @brief Paints one beam: its bands from the edge in, each a rectangle
 * from the nose to its reach. */
void paintBeam(IInterfaceCanvas& canvas, const IntroFrame& frame,
               const Beam& beam) {
  const float reach = (static_cast<float>(INTERFACE_AREA.size.x) +
                       INTRO_BEAM_OVERSHOOT - beam.nose.x) *
                      std::min(1.0F, beam.through / INTRO_BEAM_REACH_TIME);
  const float fade =
      beam.through > INTRO_BEAM_FADE_START
          ? (1.0F - beam.through) / (1.0F - INTRO_BEAM_FADE_START)
          : 1.0F;
  const int half = std::max(
      0,
      static_cast<int>(std::round(
          (INTRO_BEAM_HALF + std::sin(frame.scene * INTRO_BEAM_WAVE)) * fade)));
  const PlayerColors& colors = PLAYER_COLORS.at(beam.pilot);
  const std::array<sf::Color, INTRO_BEAM_BANDS> bands{
      INTRO_PLAYER_DARK_COLORS.at(beam.pilot), colors.ship, colors.light,
      STAR_WHITE};
  for (int level = half; level >= 0; --level) {
    const auto band = static_cast<std::size_t>(
        std::max(0, static_cast<int>(bands.size()) - 1 - level));
    paintWorldArea(
        canvas, frame,
        WorldArea{
            .topLeft =
                beam.nose - sf::Vector2f(0.0F, static_cast<float>(level)),
            .size = {static_cast<int>(std::round(reach)), level + level + 1},
            .color = bands.at(band)});
  }
}

/** @returns The remainder of @p value by @p divisor, from 0 to @p divisor,
 * also for a negative value. */
float positiveRemainder(float value, float divisor) {
  return std::fmod(std::fmod(value, divisor) + divisor, divisor);
}

/** @returns Where speed line @p line lies at @p real seconds of the music, in
 * pixels of the canvas. */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
sf::IntRect speedLineAt(std::size_t line, float real) {
  const auto seed = static_cast<double>(line);
  const int row = static_cast<int>(introHash(seed * INTRO_LINE_ROW_SEED) *
                                   static_cast<float>(INTERFACE_AREA.size.y));
  const int length = INTRO_LINE_SHORTEST +
                     static_cast<int>(introHash(seed * INTRO_LINE_LENGTH_SEED) *
                                      INTRO_LINE_LENGTH_SPREAD);
  const float speed =
      INTRO_LINE_SLOWEST +
      (introHash(seed * INTRO_LINE_SPEED_SEED) * INTRO_LINE_SPEED_SPREAD);
  const float column =
      positiveRemainder(
          (introHash(seed * INTRO_LINE_START_SEED) * INTRO_LINE_LOOP) -
              (real * speed),
          INTRO_LINE_LOOP) -
      INTRO_LINE_LEAD;
  return {{static_cast<int>(std::round(column)), row}, {length, 1}};
}

}  // namespace

void paintCharges(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  if (frame.scene < INTRO_SCENE_CHARGE ||
      frame.scene >= INTRO_SCENE_RELEASE + INTRO_CHARGE_AFTERGLOW) {
    return;
  }
  for (std::size_t pilot = 0; pilot < INTRO_HOMES.size(); ++pilot) {
    if (const std::optional<sf::Vector2f> place =
            shipPlaceAt(pilot, frame.scene)) {
      const ChargingNose nose{.pilot = pilot, .point = noseOf(*place)};
      paintGrains(canvas, frame, nose);
      paintChargeBall(canvas, frame, nose);
    }
  }
}

void paintBeams(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  const float through = (frame.scene - INTRO_SCENE_RELEASE) / INTRO_BEAM_TIME;
  if (through < 0.0F || through >= 1.0F) {
    return;
  }
  for (std::size_t pilot = 0; pilot < INTRO_HOMES.size(); ++pilot) {
    if (const std::optional<sf::Vector2f> place =
            shipPlaceAt(pilot, frame.scene)) {
      paintBeam(
          canvas, frame,
          Beam{.pilot = pilot, .nose = noseOf(*place), .through = through});
    }
  }
}

void paintSpeedLines(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  if (frame.preference == FlashPreference::Reduced) {
    return;
  }
  const float amount =
      std::clamp((skyFactorAt(frame.scene) - INTRO_SPEED_LINE_FLOOR) /
                     INTRO_SPEED_LINE_RANGE,
                 0.0F, 1.0F);
  const auto count =
      static_cast<std::size_t>(std::round(INTRO_SPEED_LINE_COUNT * amount));
  for (std::size_t line = 0; line < count; ++line) {
    canvas.fill(
        speedLineAt(line, frame.real),
        INTRO_SPEED_LINE_COLORS.at(line % INTRO_SPEED_LINE_COLORS.size()));
  }
}

}  // namespace rtype::client
