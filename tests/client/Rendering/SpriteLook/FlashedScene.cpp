#include "FlashedScene.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "HitFlash.hpp"
#include "HitFlashSystemTestConstants.hpp"
#include "Layer.hpp"
#include "Sprite.hpp"
#include "SpriteSheetTestConstants.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

using rtype::client::HitFlash;
using rtype::client::SpriteTone;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::Layer;
using rtype::game::Sprite;

FlashedScene::FlashedScene() : components_{entities_} {}

Entity FlashedScene::flashed(Duration remaining) {
  const Entity entity = entities_.create();
  components_.add(
      entity,
      Sprite{.assetId = SHEET_ID, .frame = FRAME, .layer = Layer::Enemies});
  components_.add(entity,
                  HitFlash{.tone = SpriteTone::Flash, .remaining = remaining});
  return entity;
}

void FlashedScene::play(Duration elapsed) {
  system_.update(components_, elapsed);
}

bool FlashedScene::isFlashed(Entity entity) const {
  return components_.has<HitFlash>(entity);
}

bool FlashedScene::isAlive(Entity entity) const {
  return entities_.isAlive(entity);
}
