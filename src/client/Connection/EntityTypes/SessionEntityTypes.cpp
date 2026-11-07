#include "SessionEntityTypes.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "DrawableTypeCheck.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeEntry.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

SessionEntityTypes::SessionEntityTypes(DrawableTypeCheck drawable)
    : drawable_(std::move(drawable)) {}

std::vector<std::string> SessionEntityTypes::undrawableNamesOf(
    std::span<const network::EntityTypeEntry> types) const {
  std::vector<std::string> names;
  for (const network::EntityTypeEntry& entry : types) {
    if (!drawable_(entry.name)) {
      names.push_back(entry.name);
    }
  }
  return names;
}

void SessionEntityTypes::keep(std::vector<network::EntityTypeEntry> types,
                              network::ShipGun gun) {
  types_ = std::move(types);
  gun_ = gun;
}

void SessionEntityTypes::forget() {
  types_.clear();
  gun_ = network::ShipGun{};
}

network::ShipGun SessionEntityTypes::shipGun() const { return gun_; }

const network::EntityTypeEntry* SessionEntityTypes::entryNumbered(
    std::uint8_t number) const {
  if (number == identifiers::NO_ENTITY_TYPE ||
      static_cast<std::size_t>(number) > types_.size()) {
    return nullptr;
  }
  return &types_[static_cast<std::size_t>(number) - 1];
}

}  // namespace rtype::client
