#pragma once

#include <SFML/System/Vector2.hpp>
#include <random>
#include "IPixelSurface.hpp"
#include "ISkyEvent.hpp"

namespace rtype::client {

/**
 * @brief A star that flares up into a cross, its arms growing then shrinking
 * along half a sine wave, before it fades out.
 */
class Flare final : public ISkyEvent {
 public:
  explicit Flare(std::mt19937& random);

  void advance(float eventSeconds) override;
  [[nodiscard]] bool isOver() const override;
  void paint(IPixelSurface& surface) const override;

 private:
  sf::Vector2i center_;
  float duration_;
  float age_ = 0.0F;
};

}  // namespace rtype::client
