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
#include "AccountLinkState.hpp"
#include "ExchangeRequest.hpp"
#include "HttpStatusConstants.hpp"
#include "MasterAccountClient.hpp"
#include "MasterCalls.hpp"
#include "NetworkException.hpp"
#include "OwnedLooks.hpp"
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
 * @p status, a status both routes answer alike. */
WalletRefusal walletRefusalOf(int status) {
  switch (status) {
    case network::CONFLICT_STATUS:
      return WalletRefusal::BalanceRefused;
    case network::UNAUTHORIZED_STATUS:
      return WalletRefusal::SignedOut;
    default:
      return WalletRefusal::MasterRefused;
  }
}

/** @returns Why the master refused a purchase as @p refused says: 422 for a
 * look the shop does not sell, 404 for a name no look has. */
WalletRefusal purchaseRefusalOf(
    const network::MasterRefusedException& refused) {
  switch (refused.status()) {
    case network::UNPROCESSABLE_CONTENT_STATUS:
      return WalletRefusal::NotForSale;
    case network::NOT_FOUND_STATUS:
      return WalletRefusal::UnknownItem;
    default:
      return walletRefusalOf(refused.status());
  }
}

/** @returns Why the master refused an exchange as @p refused says: 422 for
 * an amount of none. */
WalletRefusal exchangeRefusalOf(
    const network::MasterRefusedException& refused) {
  return refused.status() == network::UNPROCESSABLE_CONTENT_STATUS
             ? WalletRefusal::InvalidAmount
             : walletRefusalOf(refused.status());
}

/** @returns The account @p client reads for the ask of @p asks, or why it
 * reads none. */
UserReading userReadingOf(network::MasterAccountClient& client,
                          const AccountAsks& asks) {
  UserReading reading{.ask = asks.userAsk, .user = {}, .refusal = {}};
  reading.refusal = masterRefusalOf<SignInRefusal>(
      [&client, &asks, &reading] {
        reading.user = client.readOwnUser(asks.userToken);
      },
      [](const network::MasterRefusedException& refused) {
        return signInRefusalOf(refused.status());
      });
  return reading;
}

/** @returns The balances @p change leaves, numbered @p ask, or why it was
 * refused, as @p refusedOf reads a refusal of its route. */
template <typename Change, typename RefusedOf>
WalletChangeReading walletChangeOf(std::uint64_t ask, Change change,
                                   RefusedOf refusedOf) {
  WalletChangeReading reading{.ask = ask, .wallet = {}, .refusal = {}};
  reading.refusal = masterRefusalOf<WalletRefusal>(
      [&change, &reading] { reading.wallet = change(); }, refusedOf);
  return reading;
}

/** @brief Reads with @p client the wallet of the token of @p asks, and
 * publishes it in @p state. */
void readWallet(network::MasterAccountClient& client, const AccountAsks& asks,
                AccountLinkState& state) {
  state.wallet.publish(WalletReading{
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

/** @returns The looks the wallet @p state read for the token of @p asks,
 * which an exchange leaves as they were; read with @p client when none was
 * read for that token. */
std::optional<network::OwnedLooks> looksKept(
    network::MasterAccountClient& client, const AccountAsks& asks,
    const AccountLinkState& state) {
  const std::optional<WalletReading> known = state.wallet.value();
  if (known.has_value() && known->token == asks.token &&
      known->looks.has_value()) {
    return known->looks;
  }
  return readingOrNothing(
      [&client, &asks] { return client.listOwnLooks(asks.token); });
}

/** @brief Buys the look of @p asks when the purchase is newer than the one
 * @p state served; once made, publishes the balances it answered with the
 * looks read again. */
void servePurchase(network::MasterAccountClient& client,
                   const AccountAsks& asks, AccountLinkState& state) {
  if (asks.purchaseAsk <= state.served.purchaseAsk) {
    return;
  }
  const WalletChangeReading bought = walletChangeOf(
      asks.purchaseAsk,
      [&client, &asks] {
        return client.buyOwnLook(asks.token, asks.purchaseItem).wallet;
      },
      purchaseRefusalOf);
  state.purchase.publish(bought);
  state.served.purchaseAsk = asks.purchaseAsk;
  if (bought.wallet.has_value()) {
    state.wallet.publish(
        WalletReading{.token = asks.token,
                      .wallet = bought.wallet,
                      .looks = readingOrNothing([&client, &asks] {
                        return client.listOwnLooks(asks.token);
                      })});
  }
}

/** @brief Makes the exchange of @p asks when it is newer than the one
 * @p state served; once made, publishes the balances it answered with the
 * looks kept. */
void serveExchange(network::MasterAccountClient& client,
                   const AccountAsks& asks, AccountLinkState& state) {
  if (asks.exchangeAsk <= state.served.exchangeAsk) {
    return;
  }
  const WalletChangeReading exchanged = walletChangeOf(
      asks.exchangeAsk,
      [&client, &asks] {
        return client.exchange(asks.token, asks.exchangeRequest).wallet;
      },
      exchangeRefusalOf);
  state.exchange.publish(exchanged);
  state.served.exchangeAsk = asks.exchangeAsk;
  if (exchanged.wallet.has_value()) {
    state.wallet.publish(
        WalletReading{.token = asks.token,
                      .wallet = exchanged.wallet,
                      .looks = looksKept(client, asks, state)});
  }
}

/** @brief Answers the purchase and the exchange of @p asks newer than those
 * @p state served, each publishing the balances it left, then the wallet
 * when it was asked. */
void serveWallet(network::MasterAccountClient& client, const AccountAsks& asks,
                 AccountLinkState& state) {
  servePurchase(client, asks, state);
  serveExchange(client, asks, state);
  if (asks.walletAsk > state.served.walletAsk) {
    readWallet(client, asks, state);
  }
}

}  // namespace

void MasterAccountLink::start(const std::string& url) {
  auto client = std::make_unique<network::MasterAccountClient>(url);
  thread_ = std::jthread(
      [this, client = std::move(client)](const std::stop_token& stop) {
        serveAsksUntilStopped(stop, state_.asks,
                              [this, &client](const AccountAsks& asks) {
                                serveUser(*client, asks, state_);
                                serveWallet(*client, asks, state_);
                              });
      });
}

void MasterAccountLink::stop() { thread_ = std::jthread{}; }

std::uint64_t MasterAccountLink::askUser(const std::string& token) {
  ++asked_.userAsk;
  asked_.userToken = token;
  state_.asks.publish(asked_);
  return asked_.userAsk;
}

void MasterAccountLink::askWallet(const std::string& token) {
  ++asked_.walletAsk;
  asked_.token = token;
  state_.asks.publish(asked_);
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
