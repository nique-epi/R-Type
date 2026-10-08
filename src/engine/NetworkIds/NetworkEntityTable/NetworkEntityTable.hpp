#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include "Entity.hpp"

namespace rtype::engine {

/**
 * @brief Finds the local entity of a network identifier, on the client.
 * It does not know whether an entity is still alive: whoever destroys an
 * entity unbinds its identifier.
 */
class NetworkEntityTable {
 public:
  /**
   * @brief Associates the network identifier with the local entity.
   * @returns true when bound; false when nothing changed, because the
   * identifier was already bound (the existing association is kept) or is
   * NO_NETWORK_ID.
   */
  bool bind(std::uint32_t networkId, engine::Entity entity);

  /**
   * @returns The local entity, or an empty optional when the identifier is
   * unknown.
   */
  [[nodiscard]] std::optional<engine::Entity> find(
      std::uint32_t networkId) const;

  /**
   * @returns true when an association was removed; false when the identifier
   * was unknown.
   */
  bool unbind(std::uint32_t networkId);

  /** @returns The number of associations. */
  [[nodiscard]] std::size_t size() const;

 private:
  std::unordered_map<std::uint32_t, engine::Entity> entityByNetworkId_;
};

}  // namespace rtype::engine
