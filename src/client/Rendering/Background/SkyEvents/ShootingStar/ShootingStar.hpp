#pragma once

#include <SFML/System/Vector2.hpp>
#include <random>
#include "IPixelSurface.hpp"
#include "ISkyEvent.hpp"

namespace rtype::client {

/**
 * @brief A streak falling towards the bottom-left. Its trail grows up to
 * SHOOTING_STAR_LONGEST_TRAIL pixels while it lives, then shrinks to nothing.
 */
class ShootingStar final : public ISkyEvent {
 public:
  explicit ShootingStar(std::mt19937& random);

  void advance(float eventSeconds) override;
  [[nodiscard]] bool isOver() const override;
  void paint(IPixelSurface& surface) const override;

 private:
  float speed_;
  /** Unit vector of its flight, heading left and down. */
  sf::Vector2f direction_;
  sf::Vector2f head_;
  float remainingLife_;
  float trailLength_ = 0.0F;
};

}  // namespace rtype::client
