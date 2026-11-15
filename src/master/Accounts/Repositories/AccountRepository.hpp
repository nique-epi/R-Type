#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include "AccountRefusal.hpp"
#include "AccountRole.hpp"
#include "CreationQuota.hpp"
#include "CredentialsRequest.hpp"
#include "MasterDatabase.hpp"
#include "NewAccount.hpp"
#include "StoredAccount.hpp"

namespace rtype::master {

/** @brief The table `accounts`: the only code that writes its SQL. Each call
 * runs behind the lock of the MasterDatabase. */
class AccountRepository {
 public:
  explicit AccountRepository(MasterDatabase& database);

  /**
   * @returns Why the account @p request asks for is refused already, before
   * its password is hashed: its address created @p quota maximum accounts or
   * more since @p quota since, or another account has its nickname; nothing
   * when neither holds. One read, which insertWithin() checks again.
   */
  [[nodiscard]] std::optional<AccountRefusal> refusalBeforeHashing(
      const CredentialsRequest& request, const CreationQuota& quota);

  /**
   * @brief Adds @p account as a player, unless its address created
   * @p quota maximum accounts or more since @p quota since, or another
   * account has its nickname; the count and the insert are one transaction,
   * so requests at once cannot pass the quota or take one nickname together.
   * @returns The id of the account added, or why it was not.
   */
  std::variant<std::int64_t, AccountRefusal> insertWithin(
      const NewAccount& account, const CreationQuota& quota);

  /** @returns The account named @p nickname, compared without the case of its
   * ASCII letters, or nothing. */
  [[nodiscard]] std::optional<StoredAccount> accountNamed(
      const std::string& nickname);

  /** @brief Gives the account named @p nickname the role @p role.
   * @returns Whether an account has that nickname. */
  bool giveRole(const std::string& nickname, network::AccountRole role);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
