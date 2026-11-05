#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include "DrawableTypeCheck.hpp"
#include "EntityTypeEntry.hpp"
#include "IEntityTypeTable.hpp"

namespace rtype::client {

/**
 * @brief The table of the entity types of the session: the one the
 * ConnectionAccepted that opened it carried, kept until the session closes.
 *
 * It judges a table before the session keeps it: HandshakeAnswers asks it
 * which names of the acceptance this client cannot draw, and leaves the
 * session instead of opening it when there is one. The acceptance carries the
 * table, so a session never holds without one. Not thread-safe: use it from
 * the frame loop only.
 */
class SessionEntityTypes final : public IEntityTypeTable {
 public:
  /** @param drawable Asked once for each name a table holds. Must not be
   * empty. */
  explicit SessionEntityTypes(DrawableTypeCheck drawable);

  /** @returns The names of @p types the DrawableTypeCheck refuses, in their
   * order; empty when this client draws them all. */
  [[nodiscard]] std::vector<std::string> undrawableNamesOf(
      std::span<const network::EntityTypeEntry> types) const;

  /** @brief Keeps @p types as the table of the session, in place of any. */
  void keep(std::vector<network::EntityTypeEntry> types);

  /** @brief Forgets the table: the session closed. */
  void forget();

  [[nodiscard]] const network::EntityTypeEntry* entryNumbered(
      std::uint8_t number) const override;

 private:
  DrawableTypeCheck drawable_;
  std::vector<network::EntityTypeEntry> types_;
};

}  // namespace rtype::client
