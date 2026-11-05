#pragma once

#include <cstdint>
#include <optional>
#include "EntityTypeEntry.hpp"

namespace rtype::client {

/**
 * @brief The entity types the server of the session plays, as the table of
 * its ConnectionAccepted names them: the number an EntitySpawned or a state
 * carries, its side and the name of its sheet.
 */
class IEntityTypeTable {
 public:
  virtual ~IEntityTypeTable() = default;

  /**
   * @returns The entry of the type numbered @p number, the first entry being
   * number 1; nothing outside a session, for NO_ENTITY_TYPE, or for a number
   * past the last entry.
   */
  [[nodiscard]] virtual std::optional<network::EntityTypeEntry> entryNumbered(
      std::uint8_t number) const = 0;
};

}  // namespace rtype::client
