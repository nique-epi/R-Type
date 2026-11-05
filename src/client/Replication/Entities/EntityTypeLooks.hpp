#pragma once

#include <cstdint>
#include <optional>
#include "EntityAppearances.hpp"
#include "EntityTypeLook.hpp"
#include "IEntityTypeSource.hpp"
#include "IEntityTypeTable.hpp"

namespace rtype::client {

/**
 * @brief The look of each entity type the server of the session numbers: the
 * side the table of the server gives it, and the appearance of the sheet of
 * its name. It keeps nothing: each call reads the table of the session as it
 * stands, so a new session numbers its types by the table of its own
 * acceptance.
 */
class EntityTypeLooks final : public IEntityTypeSource {
 public:
  /**
   * @param table The table of the server; it must outlive this object.
   * @param appearances The appearances of the sheets of the client; they must
   * outlive this object and every look it gives.
   */
  EntityTypeLooks(const IEntityTypeTable& table,
                  const EntityAppearances& appearances);

  [[nodiscard]] std::optional<EntityTypeLook> lookOf(
      std::uint8_t number) const override;

 private:
  const IEntityTypeTable* table_;
  const EntityAppearances* appearances_;
};

}  // namespace rtype::client
