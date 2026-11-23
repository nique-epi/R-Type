#include "FixedEntityTypeTable.hpp"
#include <cstddef>
#include <cstdint>
#include "EntityAppearances.hpp"
#include "EntityType.hpp"
#include "FixedEntityTypeTableConstants.hpp"
#include "NumberedEntityType.hpp"
#include "ShipArsenal.hpp"

namespace rtype::tools::testing {

FixedEntityTypeTable::FixedEntityTypeTable()
    : types_{game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::Enemy,
                                      .hitbox = {.width = FIXED_ENEMY_WIDTH,
                                                 .height = FIXED_ENEMY_HEIGHT},
                                      .hitboxParts = {},
                                      .mask = nullptr},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::EnemyMissile,
                                      .hitboxParts = {},
                                      .mask = nullptr},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::LookItem,
                                      .hitboxParts = {},
                                      .mask = nullptr},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::PlayerMissile,
                                      .hitboxParts = {},
                                      .mask = nullptr},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::Player,
                                      .hitboxParts = {},
                                      .mask = nullptr},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::Enemy,
                                      .hitbox = {.width = FIXED_BOSS_WIDTH,
                                                 .height = FIXED_BOSS_HEIGHT},
                                      .hitboxParts = {},
                                      .mask = nullptr}},
      appearances_(client::EntityAppearances::fromTexts({})) {}

const game::NumberedEntityType* FixedEntityTypeTable::entryNumbered(
    std::uint8_t number) const {
  if (number == 0 || number > types_.size()) {
    return nullptr;
  }
  return &types_.at(static_cast<std::size_t>(number) - 1);
}

const client::EntityAppearances& FixedEntityTypeTable::appearances() const {
  return appearances_;
}

const game::ShipArsenal& FixedEntityTypeTable::shipArsenal() const {
  return arsenal_;
}

}  // namespace rtype::tools::testing
