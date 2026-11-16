#pragma once

#include <optional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "SettingValue.hpp"
#include "Wallet.hpp"
#include "WalletController.hpp"
#include "WalletService.hpp"

namespace rtype::master {

/**
 * @brief The module of the wallets of the accounts: their kopecks and chips,
 * the journal of what moved them, and the looks they own, kept in the tables
 * `wallets`, `wallet_movements`, `owned_looks` and `credited_results`. It
 * answers `GET /api/users/me/wallet`, `GET /api/users/me/wallet/movements`,
 * `POST /api/users/me/wallet/exchanges`,
 * `GET /api/users/me/wallet/exchanges/{exchange}`, `GET /api/users/me/looks`
 * and `PUT /api/users/me/looks/{item}`, over HTTPS only, to the account of a
 * session; and provides IAccountWallet to the other modules.
 *
 * It needs the module accounts, which names the account of a session: a
 * master without accounts turns it off first, `module wallet off`. install()
 * declares `wallet-starting-kopecks` and `wallet-starting-chips`, read at each
 * opening of a wallet. The module runs no thread.
 */
class WalletModule final : public engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Declares the settings of the starting balances, provides
   * IAccountWallet and adds the routes of the wallets; opens nothing. */
  void install(engine::InstallContext& context) override;

  void start() override;
  void stop() override;

 private:
  void declareSettings(engine::ConfigurationRegistry& configuration);

  /** @returns The balances a wallet opens with, as the settings hold them. */
  [[nodiscard]] network::Wallet startingWallet() const;

  const engine::SettingValue* startingKopecks_{nullptr};
  const engine::SettingValue* startingChips_{nullptr};
  std::optional<WalletService> service_;
  std::optional<WalletController> controller_;
};

}  // namespace rtype::master
