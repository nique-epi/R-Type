#include "FixedEntityTypeTable.hpp"
#include <cstddef>
#include <cstdint>
#include "EntityAppearances.hpp"
#include "EntityType.hpp"
#include "FixedEntityTypeTableConstants.hpp"
#include "NumberedEntityType.hpp"
#include "ShipGun.hpp"

namespace rtype::tools::testing {

FixedEntityTypeTable::FixedEntityTypeTable()
    : types_{game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::Enemy,
                                      .hitbox = {.width = FIXED_ENEMY_WIDTH,
                                                 .height = FIXED_ENEMY_HEIGHT}},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::EnemyMissile},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::LookItem},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::PlayerMissile},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::Player},
             game::NumberedEntityType{.name = {},
                                      .side = game::EntityType::Enemy,
                                      .hitbox = {.width = FIXED_BOSS_WIDTH,
                                                 .height = FIXED_BOSS_HEIGHT}}},
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

game::ShipGun FixedEntityTypeTable::shipGun() const { return game::ShipGun{}; }

}  // namespace rtype::tools::testing
