#include "NetworkEntityTable.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "Entity.hpp"
#include "NetworkIdConstants.hpp"

namespace rtype::engine {

bool NetworkEntityTable::bind(std::uint32_t networkId, Entity entity) {
  if (networkId == NO_NETWORK_ID) {
    return false;
  }
  return entityByNetworkId_.try_emplace(networkId, entity).second;
}

std::optional<Entity> NetworkEntityTable::find(std::uint32_t networkId) const {
  const auto found = entityByNetworkId_.find(networkId);
  if (found == entityByNetworkId_.end()) {
    return std::nullopt;
  }
  return found->second;
}

bool NetworkEntityTable::unbind(std::uint32_t networkId) {
  return entityByNetworkId_.erase(networkId) > 0;
}

std::size_t NetworkEntityTable::size() const {
  return entityByNetworkId_.size();
}

}  // namespace rtype::engine
