#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include "AccountAsks.hpp"
#include "AccountLinkState.hpp"
#include "ExchangeRequest.hpp"
#include "IAccountLink.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

namespace rtype::client {

/**
 * @brief The calls to the account of a session on a thread of their own:
 * network::MasterAccountClient blocks for as long as HTTP takes,
 * MASTER_TRANSFER_TIMEOUT at most per call, and the frame loop must never wait
 * on it.
 *
 * The frame loop publishes its asks; the thread sleeps until a newer one,
 * serves the newest of each kind only, the account first, then a purchase, an
 * exchange and the wallet, and publishes what the master answered. A purchase
 * or an exchange made reads the wallet again, so the balances and the looks
 * follow it. A master that does not answer, refuses or writes what cannot be
 * read gives a reading that says so, never no reading.
 *
 * Building it opens nothing; start() builds the client of the master, which
 * checks its url, and starts the thread, which owns it; stop() ends the
 * thread, a call in progress ended within the timeouts of the client. The
 * asks and the readings are safe from both threads.
 */
class MasterAccountLink final : public IAccountLink {
 public:
  MasterAccountLink() = default;
  ~MasterAccountLink() override = default;
  MasterAccountLink(const MasterAccountLink&) = delete;
  MasterAccountLink& operator=(const MasterAccountLink&) = delete;
  MasterAccountLink(MasterAccountLink&&) = delete;
  MasterAccountLink& operator=(MasterAccountLink&&) = delete;

  /** @brief Starts the thread on the master at @p url.
   * @throws network::InvalidMasterUrlException when the url is not
   * `http(s)://<host>[:<port>]`; the thread does not start then. */
  void start(const std::string& url);

  /** @brief Ends the thread, and with it its client. */
  void stop();

  std::uint64_t askUser(const std::string& token) override;
  std::uint64_t askWallet(const std::string& token) override;
  std::uint64_t askPurchase(const std::string& token,
                            std::string_view item) override;
  std::uint64_t askExchange(const std::string& token,
                            const network::ExchangeRequest& request) override;
  [[nodiscard]] std::optional<UserReading> user() const override;
  [[nodiscard]] std::optional<WalletReading> wallet() const override;
  [[nodiscard]] std::optional<WalletChangeReading> purchase() const override;
  [[nodiscard]] std::optional<WalletChangeReading> exchange() const override;

 private:
  AccountAsks asked_;
  AccountLinkState state_;
  std::jthread thread_;
};

}  // namespace rtype::client
