#include "EntityTypeLooks.hpp"
#include <cstdint>
#include <optional>
#include "EntityAppearance.hpp"
#include "EntityAppearances.hpp"
#include "EntitySide.hpp"
#include "EntityType.hpp"
#include "EntityTypeEntry.hpp"
#include "EntityTypeLook.hpp"
#include "IEntityTypeTable.hpp"

namespace rtype::client {

namespace {

/**
 * @returns The side of the game a side of the wire names. The reader of
 * ConnectionAccepted refuses any other value, so the last return is never
 * reached.
 */
game::EntityType gameSideOf(network::EntitySide side) {
  switch (side) {
    case network::EntitySide::Player:
      return game::EntityType::Player;
    case network::EntitySide::Enemy:
      return game::EntityType::Enemy;
    case network::EntitySide::PlayerMissile:
      return game::EntityType::PlayerMissile;
    case network::EntitySide::EnemyMissile:
      return game::EntityType::EnemyMissile;
  }
  return game::EntityType::Enemy;
}

}  // namespace

EntityTypeLooks::EntityTypeLooks(const IEntityTypeTable& table,
                                 const EntityAppearances& appearances)
    : table_(&table), appearances_(&appearances) {}

std::optional<EntityTypeLook> EntityTypeLooks::lookOf(
    std::uint8_t number) const {
  const network::EntityTypeEntry* const entry = table_->entryNumbered(number);
  if (entry == nullptr) {
    return std::nullopt;
  }
  const EntityAppearance* const appearance = appearances_->named(entry->name);
  if (appearance == nullptr) {
    return std::nullopt;
  }
  return EntityTypeLook{.side = gameSideOf(entry->side),
                        .appearance = *appearance};
}

}  // namespace rtype::client
