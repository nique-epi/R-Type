#pragma once

#include <SFML/System/Vector2.hpp>
#include "ISky.hpp"

/** @brief Sky double that keeps the last speed factor and offset it is
 * given; it needs no graphics card. */
class RecordingSky : public rtype::client::ISky {
 public:
  void setSpeedFactor(float factor) override;
  void setOffset(sf::Vector2i pixels) override;

  /** @returns The last speed factor given, 1 before any. */
  [[nodiscard]] float lastSpeedFactor() const;
  /** @returns The last offset given, (0, 0) before any. */
  [[nodiscard]] sf::Vector2i lastOffset() const;

 private:
  float speedFactor_{1.0F};
  sf::Vector2i offset_;
};
