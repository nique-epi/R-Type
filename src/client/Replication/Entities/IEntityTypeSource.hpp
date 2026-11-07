#pragma once

#include <cstdint>
#include <optional>
#include "EntityTypeLook.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/**
 * @brief Where the replicator finds what an entity type number of the server
 * means: the side and the appearance of the type.
 */
class IEntityTypeSource {
 public:
  virtual ~IEntityTypeSource() = default;

  /**
   * @returns The look of the type numbered @p number, or nothing when that
   * number names no type the client can draw: no session holds a table,
   * the number is past it or NO_ENTITY_TYPE, or its sheet has no
   * appearance.
   */
  [[nodiscard]] virtual std::optional<EntityTypeLook> lookOf(
      std::uint8_t number) const = 0;

  /** @returns The gun of the ships of the session; one that predicts nothing
   * outside a session. */
  [[nodiscard]] virtual network::ShipGun shipGun() const = 0;
};

}  // namespace rtype::client
