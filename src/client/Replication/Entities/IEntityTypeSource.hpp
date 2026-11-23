#pragma once

#include <cstdint>
#include <optional>
#include "EntityTypeLook.hpp"
#include "ShipArsenal.hpp"

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

  /** @returns The guns of the ships of the session; none that predicts
   * anything outside a session. Read them at once, never keep the reference
   * past the session. */
  [[nodiscard]] virtual const game::ShipArsenal& shipArsenal() const = 0;
};

}  // namespace rtype::client
