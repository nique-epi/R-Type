#pragma once

#include <SQLiteCpp/Database.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "MovementRecord.hpp"
#include "PageRequest.hpp"
#include "WalletExchange.hpp"
#include "WalletMovement.hpp"

namespace rtype::master {

/** @brief The table `wallet_movements`, the journal of the wallets: the only
 * code that writes its SQL. A line is added, never changed. It works on a
 * connection the caller holds, inside the caller's transaction. */
class WalletMovementRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit WalletMovementRepository(SQLite::Database& connection);

  /** @returns The number the next line of the journal takes when none is
   * given: one more than the largest. Read inside the transaction that adds
   * the line, so no other line takes it first. */
  [[nodiscard]] std::int64_t nextId();

  /** @brief Adds @p record to the journal.
   * @returns Its number. */
  std::int64_t insert(const MovementRecord& record);

  /** @returns The lines of the wallet of @p userId @p page asks for, newest
   * first. */
  [[nodiscard]] std::vector<network::WalletMovement> pageOf(
      std::int64_t userId, const PageRequest& page);

  /** @returns The exchange whose line taking the amount has the number
   * @p exchangeId in the wallet of @p userId, nothing when it has none. */
  [[nodiscard]] std::optional<network::WalletExchange> exchangeOf(
      std::int64_t userId, std::int64_t exchangeId);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
