#pragma once

#include <cstdint>
#include "EntityAppearances.hpp"
#include "NumberedEntityType.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/**
 * @brief The entity types the server of the session plays, as its content
 * describes them: the number an EntitySpawned or a state carries, the side
 * and the name of its sheet, and the appearance of each.
 */
class IEntityTypeTable {
 public:
  virtual ~IEntityTypeTable() = default;

  /**
   * @returns The type numbered @p number, the first being number 1; nullptr
   * outside a session, for NO_ENTITY_TYPE, or for a number past the last
   * type. It lives in the table until the table changes: read it at once,
   * never keep the pointer.
   */
  [[nodiscard]] virtual const game::NumberedEntityType* entryNumbered(
      std::uint8_t number) const = 0;

  /**
   * @returns The appearances of the content of the session: those of the
   * client's own sheets, or those of the pack of the server. Read them at
   * once, never keep the reference past the session.
   */
  [[nodiscard]] virtual const EntityAppearances& appearances() const = 0;

  /**
   * @returns The gun of the ships of the content of the session, read from
   * its sheets (game::shipGunOrNoneOf()); one that predicts nothing when
   * they name behaviours this client does not know.
   */
  [[nodiscard]] virtual game::ShipGun shipGun() const = 0;
};

}  // namespace rtype::client
