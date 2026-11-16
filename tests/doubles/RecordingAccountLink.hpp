#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "AccountAsks.hpp"
#include "ExchangeRequest.hpp"
#include "IAccountLink.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

/**
 * @brief A link to the account that records the asks and answers what the
 * test publishes, at once: the readings a test sets stand for what the thread
 * of a MasterAccountLink would publish, any number of frames later.
 */
class RecordingAccountLink final : public rtype::client::IAccountLink {
 public:
  std::uint64_t askUser(const std::string& token) override;
  void askWallet(const std::string& token) override;
  std::uint64_t askPurchase(const std::string& token,
                            std::string_view item) override;
  std::uint64_t askExchange(
      const std::string& token,
      const rtype::network::ExchangeRequest& request) override;
  [[nodiscard]] std::optional<rtype::client::UserReading> user() const override;
  [[nodiscard]] std::optional<rtype::client::WalletReading> wallet()
      const override;
  [[nodiscard]] std::optional<rtype::client::WalletChangeReading> purchase()
      const override;
  [[nodiscard]] std::optional<rtype::client::WalletChangeReading> exchange()
      const override;

  /** @brief What the link last read, which every frame then sees. Public so a
   * test publishes a reading as the thread would. */
  std::optional<rtype::client::UserReading> userRead;
  std::optional<rtype::client::WalletReading> walletRead;
  std::optional<rtype::client::WalletChangeReading> purchaseRead;
  std::optional<rtype::client::WalletChangeReading> exchangeRead;
  /** @brief The asks made so far, newest last. */
  rtype::client::AccountAsks asked;
};
