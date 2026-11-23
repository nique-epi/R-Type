#include "WeaponKeeper.hpp"
#include <optional>
#include <utility>
#include "ComponentRegistry.hpp"
#include "CrystalTouched.hpp"
#include "EntityDestruction.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityTypeNumber.hpp"
#include "EventBus.hpp"
#include "Gun.hpp"
#include "ItemClaim.hpp"
#include "ItemClaims.hpp"
#include "ItemTouch.hpp"
#include "WeaponKeeperParts.hpp"
#include "WeaponPickup.hpp"
#include "World.hpp"

namespace rtype::server {

WeaponKeeper::WeaponKeeper(WeaponKeeperParts parts)
    : parts_(parts),
      subscription_(parts_.events.get().subscribe<game::CrystalTouched>(
          [this](const game::CrystalTouched& touched) {
            onTouched(touched);
          })) {}

WeaponKeeper::~WeaponKeeper() {
  parts_.events.get().unsubscribe(subscription_);
}

void WeaponKeeper::onTick() {
  for (const ItemClaim& winner :
       winningClaims(std::exchange(claims_, {}), parts_.roster.get())) {
    const std::optional<game::Gun> gun = armedGunOf(winner);
    if (gun.has_value()) {
      give(winner, *gun);
    }
  }
}

void WeaponKeeper::onTouched(const game::CrystalTouched& touched) {
  const std::optional<ItemClaim> claim =
      itemClaimOf(parts_.world.get(), parts_.roster.get(),
                  ItemTouch{.ship = touched.ship, .item = touched.crystal});
  if (claim.has_value()) {
    claims_.push_back(*claim);
  }
}

std::optional<game::Gun> WeaponKeeper::armedGunOf(
    const ItemClaim& winner) const {
  game::World& world = parts_.world.get();
  const game::EntitySheets& types = parts_.types.get();
  const auto* const crystalType =
      world.components().get<game::EntityTypeNumber>(winner.item);
  const auto* const shipType =
      world.shipComponentOf<game::EntityTypeNumber>(winner.shipId);
  const auto* const held = world.shipComponentOf<game::Gun>(winner.shipId);
  if (crystalType == nullptr || shipType == nullptr || held == nullptr) {
    return std::nullopt;
  }
  const game::EntitySheet* const crystal = types.numbered(crystalType->number);
  const game::EntitySheet* const ship = types.numbered(shipType->number);
  if (crystal == nullptr || ship == nullptr || !crystal->crystal.has_value()) {
    return std::nullopt;
  }
  return gunAfterPickup(*held, *ship, crystal->crystal->text);
}

void WeaponKeeper::give(const ItemClaim& winner, const game::Gun& gun) const {
  game::World& world = parts_.world.get();
  *world.shipComponentOf<game::Gun>(winner.shipId) = gun;
  world.components().destroy(winner.item);
  parts_.events.get().publish(game::EntityDestruction{
      .networkId = winner.itemId, .cause = game::DestructionCause::Removed});
}

}  // namespace rtype::server
