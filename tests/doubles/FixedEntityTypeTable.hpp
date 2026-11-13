#pragma once

#include <cstdint>
#include <vector>
#include "EntityAppearances.hpp"
#include "IEntityTypeTable.hpp"
#include "NumberedEntityType.hpp"
#include "ShipGun.hpp"

namespace rtype::tools::testing {

/** @brief A session whose types are, from number 1, an enemy of the hitbox
 * FIXED_ENEMY_WIDTH by FIXED_ENEMY_HEIGHT, an enemy missile, an item, a
 * missile of a player, a ship and a boss. */
class FixedEntityTypeTable final : public client::IEntityTypeTable {
 public:
  FixedEntityTypeTable();

  [[nodiscard]] const game::NumberedEntityType* entryNumbered(
      std::uint8_t number) const override;
  [[nodiscard]] const client::EntityAppearances& appearances() const override;
  [[nodiscard]] game::ShipGun shipGun() const override;

 private:
  std::vector<game::NumberedEntityType> types_;
  client::EntityAppearances appearances_;
};

}  // namespace rtype::tools::testing
