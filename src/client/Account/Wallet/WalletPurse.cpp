#include "WalletPurse.hpp"
#include <cstdint>
#include <optional>
#include "IWallet.hpp"
#include "Purse.hpp"
#include "ShipLocker.hpp"
#include "Wallet.hpp"

namespace rtype::client {

std::optional<Purse> walletPurse(const IWallet& wallet) {
  const std::optional<network::Wallet> balances = wallet.wallet();
  const std::optional<ShipLocker> looks = wallet.ownedLooks();
  if (!balances.has_value() || !looks.has_value()) {
    return std::nullopt;
  }
  return Purse{
      .kopecks = balances->kopecks, .chips = balances->chips, .locker = *looks};
}

std::optional<std::uint32_t> walletKopecks(const IWallet& wallet) {
  const std::optional<network::Wallet> balances = wallet.wallet();
  return balances.has_value() ? std::optional{balances->kopecks} : std::nullopt;
}

std::optional<std::uint32_t> walletChips(const IWallet& wallet) {
  const std::optional<network::Wallet> balances = wallet.wallet();
  return balances.has_value() ? std::optional{balances->chips} : std::nullopt;
}

}  // namespace rtype::client
