#include "PixelScene.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <utility>
#include <vector>
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "Particle.hpp"
#include "PixelEffects.hpp"
#include "Position.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"

using rtype::client::Particle;
using rtype::client::PixelEffects;
using rtype::client::Shockwave;
using rtype::client::SpeedLine;
using rtype::engine::Entity;
using rtype::game::Position;

PixelScene::PixelScene()
    : components_(entities_),
      pixels_(entities_, components_, EFFECT_TEST_SEED) {}

PixelEffects& PixelScene::pixels() { return pixels_; }

std::vector<Particle> PixelScene::particles() {
  std::vector<Particle> found;
  components_.forEach<Particle, Position>(
      [&found](Entity, const Particle& particle, const Position&) {
        found.push_back(particle);
      });
  return found;
}

std::vector<Shockwave> PixelScene::shockwaves() {
  std::vector<Shockwave> found;
  components_.forEach<Shockwave, Position>(
      [&found](Entity, const Shockwave& wave, const Position&) {
        found.push_back(wave);
      });
  return found;
}

std::vector<std::pair<float, sf::Color>> PixelScene::speedLines() {
  std::vector<std::pair<float, sf::Color>> found;
  components_.forEach<SpeedLine, Position>(
      [&found](Entity, const SpeedLine& line, const Position& position) {
        found.emplace_back(position.y, line.color);
      });
  std::ranges::sort(found, {}, &std::pair<float, sf::Color>::first);
  return found;
}
