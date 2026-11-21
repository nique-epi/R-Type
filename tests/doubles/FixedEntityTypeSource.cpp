#include "FixedEntityTypeSource.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include "Animation.hpp"
#include "CollisionBox.hpp"
#include "EntityAppearance.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "EntityTypeLook.hpp"
#include "FixedEntityTypeSourceConstants.hpp"
#include "MaskPixel.hpp"
#include "ObstacleContact.hpp"
#include "PixelMask.hpp"
#include "ShipGun.hpp"

using rtype::client::Animation;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::EntityAppearance;
using rtype::client::EntityTypeLook;
using rtype::game::EntityType;

namespace {

/** @returns The appearance that plays @p animation of the entity sheet. */
EntityAppearance appearancePlaying(const Animation& animation) {
  return EntityAppearance{
      .sheet = ENTITY_SHEET_ID,
      .idleFrames = {animation.frames.begin(), animation.frames.end()},
      .idleFrameDuration = animation.frameDuration,
      .actions = {},
      .enragedIdleFrames = {},
      .deathFrames = {},
      .deathFramesWithoutFlash = {},
      .effects = {},
      .looks = {},
      .bossName = {},
      .bossMusic = {},
      .sounds = {}};
}

/** @returns The side of the type the double numbers @p number, or nothing
 * for a number of no type. */
std::optional<EntityType> sideNumbered(std::uint8_t number) {
  switch (number) {
    case rtype::client::testing::DRIFTER_TYPE_NUMBER:
    case rtype::client::testing::WEAVER_TYPE_NUMBER:
    case rtype::client::testing::TWO_BOX_ENEMY_TYPE_NUMBER:
    case rtype::client::testing::HOLLOW_ENEMY_TYPE_NUMBER:
      return EntityType::Enemy;
    case rtype::client::testing::ENEMY_MISSILE_TYPE_NUMBER:
      return EntityType::EnemyMissile;
    case rtype::client::testing::PLAYER_MISSILE_TYPE_NUMBER:
      return EntityType::PlayerMissile;
    case rtype::client::testing::SHIP_TYPE_NUMBER:
      return EntityType::Player;
    case rtype::client::testing::LOOK_ITEM_TYPE_NUMBER:
      return EntityType::LookItem;
    case rtype::client::testing::BLOCKING_WALL_TYPE_NUMBER:
      return EntityType::Obstacle;
    default:
      return std::nullopt;
  }
}

/** @returns The box the double gives every type of side @p side. */
rtype::game::CollisionBox boxOfSide(EntityType side) {
  switch (side) {
    case EntityType::Player:
      return rtype::client::testing::SHIP_BOX;
    case EntityType::PlayerMissile:
      return rtype::client::testing::PLAYER_MISSILE_BOX;
    case EntityType::EnemyMissile:
      return rtype::client::testing::ENEMY_MISSILE_BOX;
    case EntityType::Enemy:
    case EntityType::LookItem:
    case EntityType::Obstacle:
      return rtype::client::testing::BYDO_BOX;
  }
  return rtype::client::testing::BYDO_BOX;
}

/** @returns The boxes the double lists for the type numbered @p number: those
 * of the enemy of two boxes, none for any other type. */
std::span<const rtype::game::CollisionBox> partsNumbered(std::uint8_t number) {
  if (number == rtype::client::testing::TWO_BOX_ENEMY_TYPE_NUMBER) {
    return rtype::client::testing::TWO_BOX_ENEMY_PARTS;
  }
  return {};
}

/** @returns What the type the double numbers @p number does to a ship: the
 * blocking wall blocks it; nothing for any other type. */
std::optional<rtype::game::ObstacleContact> obstacleNumbered(
    std::uint8_t number) {
  if (number == rtype::client::testing::BLOCKING_WALL_TYPE_NUMBER) {
    return rtype::game::ObstacleContact::Blocking;
  }
  return std::nullopt;
}

/** @returns The mask of the hollow enemy: HOLLOW_ENEMY_SIDE pixels a side,
 * its HOLLOW_ENEMY_DRAWN_ROWS top rows alone drawn. */
std::shared_ptr<const rtype::engine::PixelMask> hollowEnemyMask() {
  auto mask = std::make_shared<rtype::engine::PixelMask>(
      rtype::client::testing::HOLLOW_ENEMY_SIDE,
      rtype::client::testing::HOLLOW_ENEMY_SIDE);
  for (int row = 0; row < rtype::client::testing::HOLLOW_ENEMY_DRAWN_ROWS;
       ++row) {
    for (int column = 0; column < rtype::client::testing::HOLLOW_ENEMY_SIDE;
         ++column) {
      mask->markDrawn(rtype::engine::MaskPixel{.x = column, .y = row});
    }
  }
  return mask;
}

/** @returns The mask the double gives the type numbered @p number: the one of
 * the hollow enemy, shared by every look of it, none for any other type. */
std::shared_ptr<const rtype::engine::PixelMask> maskNumbered(
    std::uint8_t number) {
  static const std::shared_ptr<const rtype::engine::PixelMask> hollow =
      hollowEnemyMask();
  if (number == rtype::client::testing::HOLLOW_ENEMY_TYPE_NUMBER) {
    return hollow;
  }
  return nullptr;
}

}  // namespace

const EntityAppearance& appearanceOfSide(EntityType side) {
  static const EntityAppearance ship =
      appearancePlaying(rtype::client::PLAYER_SHIP_ANIMATION);
  static const EntityAppearance bydo =
      appearancePlaying(rtype::client::BYDO_ANIMATION);
  static const EntityAppearance playerMissile =
      appearancePlaying(rtype::client::PLAYER_MISSILE_ANIMATION);
  static const EntityAppearance enemyMissile =
      appearancePlaying(rtype::client::ENEMY_MISSILE_ANIMATION);
  switch (side) {
    case EntityType::Player:
      return ship;
    case EntityType::PlayerMissile:
      return playerMissile;
    case EntityType::EnemyMissile:
      return enemyMissile;
    case EntityType::Enemy:
    case EntityType::LookItem:
    case EntityType::Obstacle:
      break;
  }
  return bydo;
}

std::optional<EntityTypeLook> FixedEntityTypeSource::lookOf(
    std::uint8_t number) const {
  const std::optional<EntityType> side = sideNumbered(number);
  if (!side.has_value()) {
    return std::nullopt;
  }
  return EntityTypeLook{.side = *side,
                        .appearance = appearanceOfSide(*side),
                        .hitbox = boxOfSide(*side),
                        .hitboxParts = partsNumbered(number),
                        .mask = maskNumbered(number),
                        .obstacle = obstacleNumbered(number)};
}

rtype::game::ShipGun FixedEntityTypeSource::shipGun() const {
  return rtype::game::ShipGun{
      .shipType = rtype::client::testing::SHIP_TYPE_NUMBER,
      .projectileType = rtype::client::testing::PLAYER_MISSILE_TYPE_NUMBER,
      .intervalTicks = rtype::client::testing::SHIP_GUN_INTERVAL_TICKS,
      .firstShotTicks = 0,
      .velocityX = rtype::client::testing::PLAYER_MISSILE_SPEED,
      .velocityY = 0.0F};
}
