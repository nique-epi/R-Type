#pragma once

#include <string>
#include <vector>
#include "ICasinoChannel.hpp"
#include "IModule.hpp"
#include "IWallet.hpp"
#include "InstallContext.hpp"
#include "RouletteMemory.hpp"

namespace rtype::client {

/**
 * @brief The module of the casino: install() adds CASINO_SCREEN, the
 * entrance and the hall, the two ways of the cashier,
 * CASINO_BUY_CHIPS_SCREEN and CASINO_CASH_OUT_SCREEN, and the roulette
 * table, ROULETTE_SCREEN, to the ScreenRegistry, and addWork() follows the
 * table in the memory the roulette screens share, then settles in the chips
 * of the player what the rounds moved, each frame, whichever screen shows. It
 * opens nothing, so start() and stop() do nothing.
 */
class ClientCasinoModule final : public engine::IModule {
 public:
  /** @returns CASINO_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The modules that provide the registry of the screens, the
   * table and the account. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module provides the registry, the
   * table or the wallet.
   * @throws DuplicateScreenException if another module registered one of its
   * screens. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  ICasinoChannel* casino_{nullptr};
  IWallet* wallet_{nullptr};
  /** @brief The bets and the numbers drawn, kept while the table screen is
   * closed. */
  RouletteMemory roulette_;
};

}  // namespace rtype::client
