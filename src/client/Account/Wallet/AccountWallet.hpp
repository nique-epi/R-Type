#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "ExchangeRequest.hpp"
#include "IAccountLink.hpp"
#include "IAccountSession.hpp"
#include "IWallet.hpp"
#include "ShipLocker.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

namespace rtype::client {

/** @brief The wallet of the account an IAccountSession signed in, read and
 * spent through an IAccountLink with the token of that session. A reading
 * made with another token is never shown. */
class AccountWallet final : public IWallet {
 public:
  /** @param session The account whose wallet it is, and @p link the calls to
   * the master; both must outlive the wallet. */
  AccountWallet(const IAccountSession& session, IAccountLink& link);

  [[nodiscard]] std::optional<network::Wallet> wallet() const override;
  [[nodiscard]] std::optional<ShipLocker> ownedLooks() const override;
  void askWallet() override;
  std::uint64_t buy(std::size_t item) override;
  [[nodiscard]] std::optional<WalletChangeReading> purchase(
      std::uint64_t ask) const override;
  std::uint64_t exchange(const network::ExchangeRequest& request) override;
  [[nodiscard]] std::optional<WalletChangeReading> exchanged(
      std::uint64_t ask) const override;

 private:
  /** @returns The newest wallet the link read with the token signed in;
   * nothing signed out, or before it read one. */
  [[nodiscard]] std::optional<WalletReading> reading() const;

  const IAccountSession* session_;
  IAccountLink* link_;
};

}  // namespace rtype::client
