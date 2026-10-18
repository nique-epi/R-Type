#include "RecordingEntityLayer.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
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

void RecordingEntityLayer::paint(ComponentRegistry& components) {
  std::vector<SpriteFrame> frames;
  components.forEach<Position, Sprite>(
      [&frames](Entity, const Position&, const Sprite& sprite) {
        frames.push_back(sprite.frame);
      });
  ++record_->paintCount;
  record_->framesAtLastPaint = std::move(frames);
}

void RecordingEntityLayer::show(sf::RenderTarget& target) const {
  ++record_->showCount;
  record_->lastShowTarget = &target;
}
