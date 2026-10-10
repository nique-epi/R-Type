#pragma once

#include <cstdint>
#include <random>
#include <vector>
#include "BackgroundConstants.hpp"
#include "IPixelSurface.hpp"
#include "PlanetImage.hpp"
#include "SkyEventScheduler.hpp"
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

  /**
   * @brief Paints the sky back to front: far stars, the planet, middle and
   * near stars, then sky events.
   */
  void paint(IPixelSurface& surface) const;

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
  static void paintStar(const Star& star, std::int64_t twinkleStep,
                        IPixelSurface& surface);

  std::mt19937 random_;
  std::vector<Star> stars_;
  PlanetImage planetImage_;
  float planetLeft_ = BACKGROUND_WIDTH * PLANET_START_SHARE_OF_WIDTH;
  engine::Duration elapsed_{};
  SkyEventScheduler skyEvents_;
};

}  // namespace rtype::client
