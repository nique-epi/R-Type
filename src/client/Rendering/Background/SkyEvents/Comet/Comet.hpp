#pragma once

#include <SFML/System/Vector2.hpp>
#include <bitset>
#include <cstddef>
#include <random>
#include "IPixelSurface.hpp"
#include "ISkyEvent.hpp"
#include "SkyEventConstants.hpp"

namespace rtype::client {

/**
 * @brief A comet crossing from right to left, its tail fading and flickering
 * behind its head.
 */
class Comet final : public ISkyEvent {
 public:
  explicit Comet(std::mt19937& random);

  void advance(float eventSeconds) override;
  [[nodiscard]] bool isOver() const override;
  void paint(IPixelSurface& surface) const override;

 private:
  /** @returns How far along the tail a pixel lies, from 0 to almost 1. */
  [[nodiscard]] static float tailFraction(std::size_t pixel);

  sf::Vector2f head_;
  sf::Vector2f velocity_;
  /** Unit vector of the velocity: the tail lies behind it. */
  sf::Vector2f direction_;
  float gapPhase_ = 0.0F;
  std::mt19937 flickerRandom_;
  /** Tail pixels hidden during the current frame. */
  std::bitset<COMET_TAIL_LENGTH> flickeredOut_;
};

}  // namespace rtype::client
