#pragma once

#include <optional>
#include <string>
#include <vector>
#include "AccountSession.hpp"
#include "AccountWallet.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

/**
 * @brief The module of the account the player plays under, required: it
 * provides IAccountSession, the account signed in and its token, and IWallet,
 * its balances and its looks on the master, both read through the
 * IAccountLink of the connection module.
 *
 * The token lives in memory only: neither the settings nor the journal ever
 * hold it. install() builds the session and the wallet; start() signs in with
 * the token given at launch (`--session`), if any; stop() does nothing, the
 * connection module ending the links. In UPDATE_STAGE, the wallet is asked
 * as soon as the session signs in to another account.
 */
class ClientAccountModule final : public engine::IModule {
 public:
  /** @param launchToken The token `--session` gave; nothing without it. */
  explicit ClientAccountModule(std::optional<std::string> launchToken);

  /** @returns ACCOUNT_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns CONNECTION_MODULE_NAME, which provides the link. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException when no module provides
   * IAccountLink. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::optional<std::string> launchToken_;
  std::optional<AccountSession> session_;
  std::optional<AccountWallet> wallet_;
};

}  // namespace rtype::client
