#include "RecordingSky.hpp"
#include <SFML/System/Vector2.hpp>
#include <string>
#include <string_view>
#include <utility>
#include "SkyLevel.hpp"

void RecordingSky::setSpeedFactor(float factor) { speedFactor_ = factor; }

void RecordingSky::setOffset(sf::Vector2i pixels) { offset_ = pixels; }

void RecordingSky::showLevel(const rtype::client::SkyLevel& level) {
  level_ = level;
}

float RecordingSky::lastSpeedFactor() const { return speedFactor_; }

sf::Vector2i RecordingSky::lastOffset() const { return offset_; }

const rtype::client::SkyLevel& RecordingSky::lastLevel() const {
  return level_;
}

std::string RecordingSky::musicOf(std::string_view biome) const {
  return biome == musicBiome_ ? music_ : std::string{};
}

void RecordingSky::giveMusic(std::string biome, std::string music) {
  musicBiome_ = std::move(biome);
  music_ = std::move(music);
}
