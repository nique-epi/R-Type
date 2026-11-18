#pragma once

#include <SQLiteCpp/Database.h>
#include <cstdint>
#include <string>
#include <vector>
#include "LookOwnership.hpp"

namespace rtype::master {

/** @brief The table `owned_looks`: the only code that writes its SQL. It works
 * on a connection the caller holds, inside the caller's transaction. */
class OwnedLookRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit OwnedLookRepository(SQLite::Database& connection);

  /** @brief Makes the look of @p ownership one its account owns.
   * @returns Whether it is new: false when the account owned it already. */
  bool insert(const LookOwnership& ownership);

  /** @returns The names of the looks @p userId owns, in no given order. */
  [[nodiscard]] std::vector<std::string> itemsOf(std::int64_t userId);

  /** @returns Whether @p userId owns the look named @p item. */
  [[nodiscard]] bool owns(std::int64_t userId, const std::string& item);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
