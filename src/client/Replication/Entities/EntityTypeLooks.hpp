#pragma once

#include <cstdint>
#include <optional>
#include "EntityTypeLook.hpp"
#include "IEntityTypeSource.hpp"
#include "IEntityTypeTable.hpp"

namespace rtype::client {

/**
 * @brief The look of each entity type the server of the session numbers: its
 * side, and the appearance of the sheet of its name, both from the content of
 * the session. It keeps nothing: each call reads the table of the session as
 * it stands, so a new session numbers its types by its own content.
 */
class EntityTypeLooks final : public IEntityTypeSource {
 public:
  /** @param table The table of the session; it must outlive this object. */
  explicit EntityTypeLooks(const IEntityTypeTable& table);

  [[nodiscard]] std::optional<EntityTypeLook> lookOf(
      std::uint8_t number) const override;

 private:
  const IEntityTypeTable* table_;
};

}  // namespace rtype::client
