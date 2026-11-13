#include "EntityTypeLooks.hpp"
#include <cstdint>
#include <optional>
#include "EntityAppearance.hpp"
#include "EntityAppearances.hpp"
#include "EntityTypeLook.hpp"
#include "IEntityTypeTable.hpp"
#include "NumberedEntityType.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

EntityTypeLooks::EntityTypeLooks(const IEntityTypeTable& table)
    : table_(&table) {}

std::optional<EntityTypeLook> EntityTypeLooks::lookOf(
    std::uint8_t number) const {
  const game::NumberedEntityType* const entry = table_->entryNumbered(number);
  if (entry == nullptr) {
    return std::nullopt;
  }
  const EntityAppearance* const appearance =
      table_->appearances().named(entry->name);
  if (appearance == nullptr) {
    return std::nullopt;
  }
  return EntityTypeLook{.side = entry->side,
                        .appearance = *appearance,
                        .hitbox = entry->hitbox,
                        .hitboxParts = entry->hitboxParts};
}

game::ShipGun EntityTypeLooks::shipGun() const { return table_->shipGun(); }

}  // namespace rtype::client
