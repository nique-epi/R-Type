#pragma once

#include <SQLiteCpp/Database.h>
#include "GameCredit.hpp"

namespace rtype::master {

/** @brief The table `credited_results`, the results of games a wallet was
 * credited with: the only code that writes its SQL. It works on a connection
 * the caller holds, inside the caller's transaction. */
class CreditedResultRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit CreditedResultRepository(SQLite::Database& connection);

  /** @brief Notes the reference of @p credit as credited.
   * @returns Whether it is new: false when it was credited before. */
  bool insert(const GameCredit& credit);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
