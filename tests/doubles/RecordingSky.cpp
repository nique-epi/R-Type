#include "RecordingSky.hpp"
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <string>
#include <utility>
#include "SkyLevel.hpp"

void RecordingSky::setSpeedFactor(float factor) { speedFactor_ = factor; }

void RecordingSky::setOffset(sf::Vector2i pixels) { offset_ = pixels; }

void RecordingSky::showLevel(const rtype::client::SkyLevel& level) {
  level_ = level;
}

std::optional<std::string> RecordingSky::biomeMusic() const { return music_; }

std::optional<std::string> RecordingSky::biomeBossMusic() const {
  return std::nullopt;
}

void RecordingSky::setBiomeMusic(std::optional<std::string> music) {
  music_ = std::move(music);
}

float RecordingSky::lastSpeedFactor() const { return speedFactor_; }

sf::Vector2i RecordingSky::lastOffset() const { return offset_; }

const rtype::client::SkyLevel& RecordingSky::lastLevel() const {
  return level_;
}
