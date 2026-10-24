#include "EffectSpawnerFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <optional>
#include <vector>
#include "Anchor.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "HitFlash.hpp"
#include "Invulnerability.hpp"
#include "Layer.hpp"
#include "Lifetime.hpp"
#include "Nudge.hpp"
#include "Particle.hpp"
#include "Position.hpp"
#include "Reveal.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

using rtype::client::Anchor;
using rtype::client::Caption;
using rtype::client::HitFlash;
using rtype::client::Invulnerability;
using rtype::client::Lifetime;
using rtype::client::Nudge;
using rtype::client::Particle;
using rtype::client::Reveal;
using rtype::client::SpriteAnimation;
using rtype::client::SpriteTone;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;

std::optional<Entity> frozenPicture(ComponentRegistry& components) {
  std::optional<Entity> found;
  components.forEach<Lifetime, Sprite>(
      [&found](Entity entity, const Lifetime&, const Sprite&) {
        found = entity;
      });
  return found;
}

Duration framesHeldFor(ComponentRegistry& components, Entity effect) {
  const SpriteAnimation* animation = components.get<SpriteAnimation>(effect);
  return animation != nullptr ? -animation->elapsed : Duration::zero();
}

SpriteTone toneOf(ComponentRegistry& components, Entity entity) {
  const HitFlash* flash = components.get<HitFlash>(entity);
  return flash != nullptr ? flash->tone : SpriteTone::Normal;
}

std::optional<Entity> anchorOf(ComponentRegistry& components, Entity effect) {
  const Anchor* anchor = components.get<Anchor>(effect);
  return anchor != nullptr ? std::optional<Entity>(anchor->entity)
                           : std::nullopt;
}

bool isNudged(const ComponentRegistry& components, Entity entity) {
  return components.has<Nudge>(entity);
}

Duration revealAgeOf(ComponentRegistry& components, Entity ship) {
  const Reveal* reveal = components.get<Reveal>(ship);
  return reveal != nullptr ? reveal->age : Duration::zero();
}

bool isSteadilyInvulnerable(ComponentRegistry& components, Entity ship) {
  const Invulnerability* invulnerability =
      components.get<Invulnerability>(ship);
  return invulnerability != nullptr && invulnerability->steady;
}

bool everyParticleGoesThrough(ComponentRegistry& components, sf::Color color) {
  bool every = true;
  components.forEach<Particle, Position>(
      [&every, color](Entity, const Particle& particle, const Position&) {
        every = every && std::ranges::find(particle.colors, color) !=
                             particle.colors.end();
      });
  return every;
}

bool anyParticleStartsWith(ComponentRegistry& components, sf::Color color) {
  bool any = false;
  components.forEach<Particle, Position>(
      [&any, color](Entity, const Particle& particle, const Position&) {
        any = any ||
              (!particle.colors.empty() && particle.colors.front() == color);
      });
  return any;
}

std::optional<Caption> caption(ComponentRegistry& components) {
  std::optional<Caption> found;
  components.forEach<Caption, Position>(
      [&found](Entity, const Caption& figure, const Position&) {
        if (!found.has_value()) {
          found = figure;
        }
      });
  return found;
}

std::vector<Entity> shownEffects(ComponentRegistry& components) {
  std::vector<Entity> shown;
  components.forEach<Position, Sprite>(
      [&shown](Entity entity, const Position&, const Sprite& sprite) {
        if (sprite.layer == Layer::Effects) {
          shown.push_back(entity);
        }
      });
  return shown;
}

Sprite drawnSprite(ComponentRegistry& components, Entity entity) {
  const Sprite* sprite = components.get<Sprite>(entity);
  return sprite != nullptr ? *sprite : Sprite{};
}

Position positionOf(ComponentRegistry& components, Entity entity) {
  const Position* position = components.get<Position>(entity);
  return position != nullptr ? *position : Position{};
}

const HitFlash* flashOf(ComponentRegistry& components, Entity entity) {
  return components.get<HitFlash>(entity);
}
