#pragma once

#include <optional>
#include <string>
#include <vector>
#include "CasinoChannel.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "RouletteMemory.hpp"

namespace rtype::client {

/**
 * @brief The module of the casino: install() adds CASINO_SCREEN, the front
 * of the casino, CASINO_HALL_SCREEN, its hall, CASINO_CASHIER_SCREEN, its
 * cashier, and the roulette table, ROULETTE_SCREEN, to the ScreenRegistry,
 * the rooms reading their pictures as they open. It owns the CasinoChannel
 * the screens play through, built on the ICasinoLink of the connection module
 * and the IAccountSession and IWallet of the account module; addWork()
 * updates it, then follows the room followed in the memory the roulette
 * screens share, each frame, whichever screen shows: the master settles the
 * chips. CASINO_SCREEN is the
 * dialog that asks to sign in for a guest. It opens nothing, so start() and
 * stop() do nothing.
 */
class ClientCasinoModule final : public engine::IModule {
 public:
  /** @returns CASINO_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The modules that provide the registry of the screens, the
   * casino and the account. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module provides the registry,
   * the link to the casino, the session or the wallet.
   * @throws DuplicateScreenException if another module registered one of its
   * screens. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::optional<CasinoChannel> casino_;
  /** @brief The bets of the round, kept while the table screen is
   * closed. */
  RouletteMemory roulette_;
};

}  // namespace rtype::client
