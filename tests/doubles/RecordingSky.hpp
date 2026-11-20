#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include <string_view>
#include "ISky.hpp"
#include "SkyLevel.hpp"

/** @brief Sky double that keeps the last speed factor, offset and level it
 * is given, and answers one music for every biome; it needs no graphics
 * card. */
class RecordingSky : public rtype::client::ISky {
 public:
  void setSpeedFactor(float factor) override;
  void setOffset(sf::Vector2i pixels) override;
  void showLevel(const rtype::client::SkyLevel& level) override;
  /** @returns The music giveMusic() named for @p biome, empty otherwise. */
  [[nodiscard]] std::string musicOf(std::string_view biome) const override;

  /** @brief Makes musicOf() answer @p music for the biome @p biome. */
  void giveMusic(std::string biome, std::string music);

  /** @returns The last speed factor given, 1 before any. */
  [[nodiscard]] float lastSpeedFactor() const;
  /** @returns The last offset given, (0, 0) before any. */
  [[nodiscard]] sf::Vector2i lastOffset() const;
  /** @returns The last level given, one without a biome before any. */
  [[nodiscard]] const rtype::client::SkyLevel& lastLevel() const;

 private:
  float speedFactor_{1.0F};
  sf::Vector2i offset_;
  rtype::client::SkyLevel level_;
  std::string musicBiome_;
  std::string music_;
};
