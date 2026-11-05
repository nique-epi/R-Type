#pragma once

#include <cstdint>
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
   * number 1; nullptr outside a session, for NO_ENTITY_TYPE, or for a number
   * past the last entry. The entry lives in the table until the table changes:
   * read it at once, never keep the pointer.
   */
  [[nodiscard]] virtual const network::EntityTypeEntry* entryNumbered(
      std::uint8_t number) const = 0;
};

}  // namespace rtype::client
