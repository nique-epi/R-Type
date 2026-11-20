#pragma once

#include <SFML/System/Vector2.hpp>
#include <optional>
#include <string>
#include "ISky.hpp"
#include "SkyLevel.hpp"

/** @brief Sky double that keeps the last speed factor, offset and level it
 * is given, and names the music a test gives it; it needs no graphics card. */
class RecordingSky : public rtype::client::ISky {
 public:
  void setSpeedFactor(float factor) override;
  void setOffset(sf::Vector2i pixels) override;
  void showLevel(const rtype::client::SkyLevel& level) override;
  [[nodiscard]] std::optional<std::string> biomeMusic() const override;

  /** @brief Names @p music as the music of the biome shown. */
  void setBiomeMusic(std::optional<std::string> music);

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
  std::optional<std::string> music_;
};
