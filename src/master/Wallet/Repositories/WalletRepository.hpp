#pragma once

#include <SQLiteCpp/Database.h>
#include <cstdint>
#include <optional>
#include "Wallet.hpp"
#include "WalletOpening.hpp"

namespace rtype::master {

/** @brief The table `wallets`: the only code that writes its SQL. It works on
 * a connection the caller holds, inside the caller's transaction. */
class WalletRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit WalletRepository(SQLite::Database& connection);

  /** @returns The balances of the wallet of @p userId, nothing before it
   * opened. */
  [[nodiscard]] std::optional<network::Wallet> balancesOf(std::int64_t userId);

  /** @brief Opens the wallet @p opening describes.
   * @returns Whether it opened: false when no account has its user id. */
  bool insertOpened(const WalletOpening& opening);

  /** @brief Writes @p wallet as the balances of the wallet of @p userId. */
  void replace(std::int64_t userId, const network::Wallet& wallet);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
