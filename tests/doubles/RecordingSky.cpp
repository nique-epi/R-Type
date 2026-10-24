#include "RecordingSky.hpp"
#include <SFML/System/Vector2.hpp>

void RecordingSky::setSpeedFactor(float factor) { speedFactor_ = factor; }

void RecordingSky::setOffset(sf::Vector2i pixels) { offset_ = pixels; }

float RecordingSky::lastSpeedFactor() const { return speedFactor_; }

sf::Vector2i RecordingSky::lastOffset() const { return offset_; }
