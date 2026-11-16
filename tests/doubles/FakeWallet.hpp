#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "ExchangeRequest.hpp"
#include "IWallet.hpp"
#include "ShipLocker.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"

/**
 * @brief A wallet whose balances and looks a test sets, which records the
 * purchases and the exchanges asked and answers each one only once the test
 * gives its answer, as the master answers on a later frame.
 */
class FakeWallet final : public rtype::client::IWallet {
 public:
  [[nodiscard]] std::optional<rtype::network::Wallet> wallet() const override;
  [[nodiscard]] std::optional<rtype::client::ShipLocker> ownedLooks()
      const override;
  void askWallet() override;
  std::uint64_t buy(std::size_t item) override;
  [[nodiscard]] std::optional<rtype::client::WalletChangeReading> purchase(
      std::uint64_t ask) const override;
  std::uint64_t exchange(
      const rtype::network::ExchangeRequest& request) override;
  [[nodiscard]] std::optional<rtype::client::WalletChangeReading> exchanged(
      std::uint64_t ask) const override;

  /** @brief What the master read last, which the screens then see; nothing
   * by default, as before the first reading. Public so a test sets them. */
  std::optional<rtype::network::Wallet> balances;
  std::optional<rtype::client::ShipLocker> looks;
  /** @brief The answers the master gave, newest last. */
  std::optional<rtype::client::WalletChangeReading> purchaseAnswer;
  std::optional<rtype::client::WalletChangeReading> exchangeAnswer;
  /** @brief The items bought and the exchanges asked, in order, and how many
   * times the wallet was asked again. */
  std::vector<std::size_t> boughtItems;
  std::vector<rtype::network::ExchangeRequest> exchangesAsked;
  std::size_t walletAsks{};
};
