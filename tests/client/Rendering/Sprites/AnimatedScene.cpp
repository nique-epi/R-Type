#include "AnimatedScene.hpp"
#include "AnimationTestConstants.hpp"
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "Layer.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "SpriteSheetTestConstants.hpp"
#include "SpriteTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::SpriteAnimation;
using rtype::engine::Duration;
using rtype::game::Layer;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

AnimatedScene::AnimatedScene(const SpriteAnimation& animation)
    : components_{entities_}, entity_(entities_.create()) {
  components_.add(
      entity_,
      Sprite{.assetId = SHEET_ID, .frame = FIRST, .layer = Layer::Enemies});
  components_.add(entity_, animation);
}

void AnimatedScene::play(Duration elapsed) {
  system_.update(components_, elapsed);
}

SpriteFrame AnimatedScene::shownFrame() {
  const Sprite* sprite = components_.get<Sprite>(entity_);
  return sprite != nullptr ? sprite->frame : NO_FRAME;
}

bool AnimatedScene::isAlive() const { return entities_.isAlive(entity_); }
