#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <vector>
#include "BackgroundConstants.hpp"
#include "IPixelSurface.hpp"
#include "PlanetImage.hpp"
#include "SkyEventScheduler.hpp"
#include "Star.hpp"
#include "StarPlane.hpp"
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
 * STREAK_SPEED_FACTOR, each is drawn with its streak behind it. Each plane
 * of STAR_PLANES is painted apart, see paint().
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
   * @brief Paints one plane of the sky: the stars of its depth, or the planet;
   * the near plane holds the sky events too.
   *
   * A slow plane is painted on whole pixels, shiftOf() left of where it lies,
   * so it can be shown that much further right.
   */
  void paint(StarPlane plane, IPixelSurface& surface) const;

  /**
   * @returns How far right of the pixels paint() uses the plane lies, from 0
   * included to 1 excluded; 0 for the near plane, which keeps the grid of the
   * playfield.
   */
  [[nodiscard]] float shiftOf(StarPlane plane) const;

  /**
   * @returns The stars: far ones first, then middle ones, then near ones.
   * Moving never changes that order, which paint() relies on to find the
   * stars of a plane.
   */
  [[nodiscard]] const std::vector<Star>& stars() const;

  /** @returns Where @p star lies, in background pixels: its column, and the
   * shift of its layer right of it. */
  [[nodiscard]] float positionOf(const Star& star) const;

  /** @returns Column of the left edge of the planet image. */
  [[nodiscard]] float planetLeft() const;

 private:
  /**
   * @brief Moves the stars of @p layer @p distance pixels left: the shift of
   * the layer moves, and each star by the whole columns it passed.
   */
  void moveLayer(const StarLayer& layer, float distance);
  void paintPlanet(IPixelSurface& surface) const;

  /** @brief Paints the stars at @p depth on their columns, each after its
   * streak when they move fast enough. */
  void paintStarsAt(StarDepth depth, IPixelSurface& surface) const;

  std::mt19937 random_;
  std::vector<Star> stars_;
  /** How far right of its column each star of a layer lies; the near stars
   * are painted on their columns all the same, see shiftOf(). */
  std::array<float, STAR_LAYERS.size()> layerShifts_{};
  PlanetImage planetImage_;
  float planetLeft_ = BACKGROUND_WIDTH * PLANET_START_SHARE_OF_WIDTH;
  engine::Duration elapsed_{};
  float speedFactor_ = 1.0F;
  SkyEventScheduler skyEvents_;
};

}  // namespace rtype::client
