#pragma once

#include <SFML/System/Vector2.hpp>
#include <random>
#include "IPixelSurface.hpp"
#include "ISkyEvent.hpp"

namespace rtype::client {

/**
 * @brief A small rock that enters on the right, drifts left while spinning
 * through ASTEROID_FRAMES, and ends once past the left edge.
 */
class Asteroid final : public ISkyEvent {
 public:
  explicit Asteroid(std::mt19937& random);

  void advance(float eventSeconds) override;
  [[nodiscard]] bool isOver() const override;
  void paint(IPixelSurface& surface) const override;

 private:
  sf::Vector2f topLeft_;
  sf::Vector2f velocity_;
  float spinSpeed_;
  /** Frame shown, as a number that grows with the spin. */
  float frame_;
};

}  // namespace rtype::client
