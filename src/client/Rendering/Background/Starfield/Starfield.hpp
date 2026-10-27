#pragma once

#include <cstdint>
#include <optional>
#include <random>
#include <vector>
#include "BackgroundConstants.hpp"
#include "IPixelSurface.hpp"
#include "PlanetImage.hpp"
#include "SkyEventScheduler.hpp"
#include "SkyLight.hpp"
#include "Star.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The scrolling sky: three layers of stars, a ringed planet and random
 * sky events, all drifting left.
 *
 * Nothing depends on the frame rate: everything moves by the time given to
 * advance(). A star that leaves on the left comes back on the right at a new
 * height, so the sky never shows a seam.
 *
 * The stars move setSpeedFactor() times their own speed, 1 until set; above
 * STREAK_SPEED_FACTOR, each is drawn with its streak behind it.
 */
class Starfield {
 public:
  /** @param seed Seed of every random draw of the sky. */
  explicit Starfield(std::uint32_t seed);

  /**
   * @brief Moves the sky by the elapsed time, capped at
   * MAXIMUM_BACKGROUND_STEP. A negative time moves nothing.
   */
  void advance(engine::Duration elapsed);

  /** @brief From the next advance, the stars move @p factor times their own
   * speed, 0 at least. */
  void setSpeedFactor(float factor);

  /**
   * @brief Paints the sky back to front: far stars, the planet, middle and
   * near stars, then sky events. In a sky of SkyLight::Steady, no star
   * twinkles and no comet tail flickers.
   */
  void paint(IPixelSurface& surface, SkyLight light) const;

  /**
   * @returns The stars: far ones first, then middle ones, then near ones.
   * Moving never changes that order, which paint() relies on to slip the
   * planet in after the far stars.
   */
  [[nodiscard]] const std::vector<Star>& stars() const;

  /** @returns Column of the left edge of the planet image. */
  [[nodiscard]] float planetLeft() const;

 private:
  void moveStar(Star& star, float seconds);
  void paintPlanet(IPixelSurface& surface) const;
  /** @brief Paints a star, twinkling when @p twinkleStep is its step of the
   * cycle; nothing twinkles without a step. */
  static void paintStar(const Star& star,
                        std::optional<std::int64_t> twinkleStep,
                        IPixelSurface& surface);

  /** @brief Paints a star, after its streak when it moves fast enough. */
  void paintMovingStar(const Star& star,
                       std::optional<std::int64_t> twinkleStep,
                       IPixelSurface& surface) const;

  /** @brief Paints, behind a star, the pixels it covers in one
   * SIMULATION_TICK_DURATION at that speed factor. */
  static void paintStreak(const Star& star, float speedFactor,
                          IPixelSurface& surface);

  std::mt19937 random_;
  std::vector<Star> stars_;
  PlanetImage planetImage_;
  float planetLeft_ = BACKGROUND_WIDTH * PLANET_START_SHARE_OF_WIDTH;
  engine::Duration elapsed_{};
  float speedFactor_ = 1.0F;
  SkyEventScheduler skyEvents_;
};

}  // namespace rtype::client
