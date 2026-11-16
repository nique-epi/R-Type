#include "MasterAccountLink.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include "AccountAsks.hpp"
#include "AccountLinkConstants.hpp"
#include "AccountLinkState.hpp"
#include "ExchangeRequest.hpp"
#include "HttpStatusConstants.hpp"
#include "MasterAccountClient.hpp"
#include "NetworkException.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

namespace {

/** @returns Why the master refused the account of a token with @p status. */
SignInRefusal signInRefusalOf(int status) {
  return status == network::UNAUTHORIZED_STATUS ? SignInRefusal::TokenRefused
                                                : SignInRefusal::MasterRefused;
}

/** @returns Why the master refused a purchase or an exchange with
 * @p status. */
WalletRefusal walletRefusalOf(int status) {
  switch (status) {
    case network::CONFLICT_STATUS:
      return WalletRefusal::BalanceRefused;
    case network::UNPROCESSABLE_CONTENT_STATUS:
      return WalletRefusal::NotForSale;
    case network::NOT_FOUND_STATUS:
      return WalletRefusal::UnknownItem;
    case network::UNAUTHORIZED_STATUS:
      return WalletRefusal::SignedOut;
    default:
      return WalletRefusal::MasterRefused;
  }
}

/** @returns The account @p client reads for the ask of @p asks, or why it
 * reads none. */
UserReading userReadingOf(network::MasterAccountClient& client,
                          const AccountAsks& asks) {
  UserReading reading{.ask = asks.userAsk, .user = {}, .refusal = {}};
  try {
    reading.user = client.readOwnUser(asks.userToken);
  } catch (const network::MasterRefusedException& refused) {
    reading.refusal = signInRefusalOf(refused.status());
  } catch (const network::MasterUnreachableException&) {
    reading.refusal = SignInRefusal::MasterUnreachable;
  } catch (const network::NetworkException&) {
    reading.refusal = SignInRefusal::MasterRefused;
  }
  return reading;
}

/** @returns The balances @p change leaves, numbered @p ask, or why it was
 * refused. */
template <typename Change>
WalletChangeReading walletChangeOf(std::uint64_t ask, Change change) {
  WalletChangeReading reading{.ask = ask, .wallet = {}, .refusal = {}};
  try {
    reading.wallet = change();
  } catch (const network::MasterRefusedException& refused) {
    reading.refusal = walletRefusalOf(refused.status());
  } catch (const network::MasterUnreachableException&) {
    reading.refusal = WalletRefusal::MasterUnreachable;
  } catch (const network::NetworkException&) {
    reading.refusal = WalletRefusal::MasterRefused;
  }
  return reading;
}

/** @returns What @p call reads, or nothing when the master does not answer,
 * refuses, or answers what cannot be read. */
template <typename Call>
auto readingOrNothing(Call call) -> std::optional<decltype(call())> {
  try {
    return call();
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
}

/** @brief Reads with @p client the wallet of the token of @p asks, and
 * publishes it in @p state. */
void readWallet(network::MasterAccountClient& client, const AccountAsks& asks,
                AccountLinkState& state) {
  state.wallet.publish(WalletReading{
      .ask = asks.walletAsk,
      .token = asks.token,
      .wallet = readingOrNothing(
          [&client, &asks] { return client.readOwnWallet(asks.token); }),
      .looks = readingOrNothing(
          [&client, &asks] { return client.listOwnLooks(asks.token); })});
  state.served.walletAsk = asks.walletAsk;
}

/** @brief Answers the ask for the account of @p asks when it is newer than
 * the one @p state served. */
void serveUser(network::MasterAccountClient& client, const AccountAsks& asks,
               AccountLinkState& state) {
  if (asks.userAsk <= state.served.userAsk) {
    return;
  }
  state.user.publish(userReadingOf(client, asks));
  state.served.userAsk = asks.userAsk;
}

/** @brief Answers the purchase and the exchange of @p asks newer than those
 * @p state served, then the wallet, read again after a change made. */
void serveWallet(network::MasterAccountClient& client, const AccountAsks& asks,
                 AccountLinkState& state) {
  bool changed = false;
  if (asks.purchaseAsk > state.served.purchaseAsk) {
    const WalletChangeReading bought =
        walletChangeOf(asks.purchaseAsk, [&client, &asks] {
          return client.buyOwnLook(asks.token, asks.purchaseItem).wallet;
        });
    state.purchase.publish(bought);
    state.served.purchaseAsk = asks.purchaseAsk;
    changed = changed || bought.wallet.has_value();
  }
  if (asks.exchangeAsk > state.served.exchangeAsk) {
    const WalletChangeReading exchanged =
        walletChangeOf(asks.exchangeAsk, [&client, &asks] {
          return client.exchange(asks.token, asks.exchangeRequest).wallet;
        });
    state.exchange.publish(exchanged);
    state.served.exchangeAsk = asks.exchangeAsk;
    changed = changed || exchanged.wallet.has_value();
  }
  if (changed || asks.walletAsk > state.served.walletAsk) {
    readWallet(client, asks, state);
  }
}

/** @brief The loop of the thread: waits for a newer ask, and serves it with
 * @p client. */
void runAccountLink(const std::stop_token& stop,
                    network::MasterAccountClient& client,
                    AccountLinkState& state) {
  std::uint64_t seen = 0;
  while (!stop.stop_requested()) {
    seen = state.asks.waitForNewer(seen, ACCOUNT_ASK_WAIT, stop);
    const std::optional<AccountAsks> asks = state.asks.value();
    if (asks.has_value()) {
      serveUser(client, *asks, state);
      serveWallet(client, *asks, state);
    }
  }
}

}  // namespace

void MasterAccountLink::start(const std::string& url) {
  auto client = std::make_unique<network::MasterAccountClient>(url);
  thread_ = std::jthread(
      [this, client = std::move(client)](const std::stop_token& stop) {
        runAccountLink(stop, *client, state_);
      });
}

void MasterAccountLink::stop() { thread_ = std::jthread{}; }

std::uint64_t MasterAccountLink::askUser(const std::string& token) {
  ++asked_.userAsk;
  asked_.userToken = token;
  state_.asks.publish(asked_);
  return asked_.userAsk;
}

std::uint64_t MasterAccountLink::askWallet(const std::string& token) {
  ++asked_.walletAsk;
  asked_.token = token;
  state_.asks.publish(asked_);
  return asked_.walletAsk;
}

std::uint64_t MasterAccountLink::askPurchase(const std::string& token,
                                             std::string_view item) {
  ++asked_.purchaseAsk;
  asked_.purchaseItem = std::string{item};
  asked_.token = token;
  state_.asks.publish(asked_);
  return asked_.purchaseAsk;
}

std::uint64_t MasterAccountLink::askExchange(
    const std::string& token, const network::ExchangeRequest& request) {
  ++asked_.exchangeAsk;
  asked_.exchangeRequest = request;
  asked_.token = token;
  state_.asks.publish(asked_);
  return asked_.exchangeAsk;
}

std::optional<UserReading> MasterAccountLink::user() const {
  return state_.user.value();
}

std::optional<WalletReading> MasterAccountLink::wallet() const {
  return state_.wallet.value();
}

std::optional<WalletChangeReading> MasterAccountLink::purchase() const {
  return state_.purchase.value();
}

std::optional<WalletChangeReading> MasterAccountLink::exchange() const {
  return state_.exchange.value();
}

}  // namespace rtype::client
