#include "RecordingEntityLayer.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "DrawJournal.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::game::Position;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

RecordingEntityLayer::RecordingEntityLayer(EntityLayerRecord& record)
    : record_(&record) {}

void RecordingEntityLayer::paint(ComponentRegistry& components,
                                 sf::Vector2i offset) {
  std::vector<SpriteFrame> frames;
  components.forEach<Position, Sprite>(
      [&frames](Entity, const Position&, const Sprite& sprite) {
        frames.push_back(sprite.frame);
      });
  ++record_->paintCount;
  record_->framesAtLastPaint = std::move(frames);
  record_->offsetAtLastPaint = offset;
}

void RecordingEntityLayer::show(sf::RenderTarget& target) const {
  ++record_->showCount;
  record_->lastShowTarget = &target;
  if (record_->drawJournal != nullptr) {
    record_->drawJournal->laid.emplace_back(ENTITY_LAYER_LAID);
  }
}
