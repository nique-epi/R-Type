#include "EffectScene.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "EffectSpawnerTestConstants.hpp"
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "FlashPreference.hpp"
#include "Layer.hpp"
#include "PilotShadeConstants.hpp"
#include "PilotShip.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "TimeConstants.hpp"

using rtype::client::EffectSpawner;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::FlashPreference;
using rtype::client::PilotShip;
using rtype::client::PLAYER_SHIP_FRAME;
using rtype::client::STANDARD_PILOT_SHADES;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::EntityType;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;

EffectScene::EffectScene()
    : components_{entities_},
      effects_{entities_, components_, EFFECT_TEST_SEED} {}

EffectSpawner& EffectScene::effects() { return effects_; }

void EffectScene::play(Duration elapsed) {
  animations_.update(components_, elapsed);
}

void EffectScene::advance(Duration elapsed, FlashPreference preference) {
  effects_.advance(elapsed, preference, STANDARD_PILOT_SHADES);
  animations_.update(components_, elapsed);
  hitFlashes_.update(components_, elapsed);
  spriteEffects_.update(components_, elapsed);
}

Entity EffectScene::shipOfPilot(int pilotColor) {
  const Entity ship = entities_.create();
  components_.add(ship, SHIP_CENTER);
  components_.add(ship, EntityType::Player);
  components_.add(ship, Sprite{.assetId = ENTITY_SHEET_ID,
                               .frame = PLAYER_SHIP_FRAME,
                               .layer = Layer::Players});
  components_.add(ship, PilotShip{.color = pilotColor});
  return ship;
}

Entity EffectScene::bydoAt(Position position) {
  const Entity bydo = entities_.create();
  components_.add(bydo, position);
  components_.add(bydo, EntityType::Enemy);
  components_.add(bydo, Sprite{.assetId = BYDO_SHEET_ID,
                               .frame = TARGET_FRAME,
                               .layer = Layer::Enemies});
  return bydo;
}

Entity EffectScene::target() {
  const Entity entity = entities_.create();
  components_.add(entity, WHERE_IT_DIED);
  components_.add(entity, Sprite{.assetId = TARGET_SHEET_ID,
                                 .frame = TARGET_FRAME,
                                 .layer = Layer::Enemies});
  return entity;
}

EntityRegistry& EffectScene::entities() { return entities_; }

ComponentRegistry& EffectScene::components() { return components_; }
