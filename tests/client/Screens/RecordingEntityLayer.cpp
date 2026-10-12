#include "RecordingEntityLayer.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "Sprite.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::game::Position;
using rtype::game::Sprite;

RecordingEntityLayer::RecordingEntityLayer(EntityLayerRecord& record)
    : record_(&record) {}

void RecordingEntityLayer::paint(ComponentRegistry& components) {
  std::size_t sprites = 0;
  components.forEach<Position, Sprite>(
      [&sprites](Entity, const Position&, const Sprite&) { ++sprites; });
  ++record_->paintCount;
  record_->spritesAtLastPaint = sprites;
}

void RecordingEntityLayer::show(sf::RenderTarget& target) const {
  ++record_->showCount;
  record_->lastShowTarget = &target;
}
