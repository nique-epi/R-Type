#include "SessionEntityTypes.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "DrawableTypeCheck.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeEntry.hpp"

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

void SessionEntityTypes::keep(std::vector<network::EntityTypeEntry> types) {
  types_ = std::move(types);
}

void SessionEntityTypes::forget() { types_.clear(); }

std::optional<network::EntityTypeEntry> SessionEntityTypes::entryNumbered(
    std::uint8_t number) const {
  if (number == identifiers::NO_ENTITY_TYPE ||
      static_cast<std::size_t>(number) > types_.size()) {
    return std::nullopt;
  }
  return types_[static_cast<std::size_t>(number) - 1];
}

}  // namespace rtype::client
