#include "EntitySprites.hpp"
#include <optional>
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

namespace {

game::Sprite entitySheetSprite(game::SpriteFrame frame) {
  return game::Sprite{.assetId = ENTITY_SHEET_ID,
                      .frame = frame,
                      .layer = game::Layer::Entities};
}

}  // namespace

std::optional<game::Sprite> spriteOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return entitySheetSprite(PLAYER_SHIP_FRAME);
    case game::EntityType::Enemy:
      return entitySheetSprite(BYDO_FRAME);
    case game::EntityType::PlayerMissile:
      return entitySheetSprite(PLAYER_MISSILE_FRAME);
    case game::EntityType::EnemyMissile:
      return entitySheetSprite(ENEMY_MISSILE_FRAME);
  }
  return std::nullopt;
}

}  // namespace rtype::client
