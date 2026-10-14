#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <random>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "MotionPreference.hpp"
#include "SquadronConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The squadron of the title screen: a box that opens, four ships that
 * fly in and take their places, then drift in a figure eight without leaving
 * the box while a random one fires now and then.
 *
 * Everything follows the time given to advance(), from the moment the squadron
 * is built, so its arrival plays each time the title screen is built. With
 * MotionPreference::Reduced, time starts and stays at SQUADRON_DRIFT_START: the
 * box is open, SQUADRON is written, the ships are in place, still, and none
 * fires. It paints through an IInterfaceCanvas, so it can be tested without a
 * graphics card.
 */
class Squadron {
 public:
  /** @param seed Seed of the choice of the ship that fires and of the gaps
   * between shots. */
  Squadron(MotionPreference motion, std::uint32_t seed);

  /** @brief Moves the squadron by the elapsed time, capped at
   * MAXIMUM_BACKGROUND_STEP as the sky is; nothing moves with
   * MotionPreference::Reduced. */
  void advance(engine::Duration elapsed);

  /**
   * @brief Paints the box, its title, the missiles, then the ships, cut off at
   * the edges of the box.
   *
   * @param topLeft Top-left corner of the box, SQUADRON_BOX_SIZE large, in
   * pixels of the interface picture.
   */
  void paint(IInterfaceCanvas& canvas, sf::Vector2i topLeft) const;

 private:
  /** @brief Where a ship is, from the top-left corner of the box, and whether
   * it has reached its place. */
  struct ShipPlacement {
    const SquadronShip* ship{};
    sf::Vector2i topLeft;
    bool landed{};
  };

  /** @returns The time of the animation, in seconds. */
  [[nodiscard]] float seconds() const;

  /** @returns The ships that have entered, at a moment of the animation. */
  [[nodiscard]] static std::vector<ShipPlacement> placements(float time);

  /** @brief Makes a random ship in place fire, then draws when the next shot
   * comes. */
  void fire(float time);

  MotionPreference motion_;
  std::mt19937 random_;
  engine::Duration elapsed_{};
  float nextShot_ = 0.0F;
  std::vector<sf::Vector2f> missiles_;
};

}  // namespace rtype::client
