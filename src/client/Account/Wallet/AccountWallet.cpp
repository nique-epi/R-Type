#include "AccountWallet.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include "AccountLinkConstants.hpp"
#include "ExchangeRequest.hpp"
#include "IAccountLink.hpp"
#include "IAccountSession.hpp"
#include "LookItems.hpp"
#include "OwnedLooks.hpp"
#include "ShipLocker.hpp"
#include "ShipLookConstants.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

namespace rtype::client {

namespace {

/** @returns The locker that holds each look of @p looks; a name no look has
 * is left out. */
ShipLocker lockerOf(const network::OwnedLooks& looks) {
  ShipLocker locker;
  locker.items.reset();
  for (const std::string& name : looks.items) {
    if (const std::optional<std::size_t> item = network::lookItemOfName(name)) {
      locker.items.set(*item);
    }
  }
  return locker;
}

/** @returns @p reading when it answers @p ask, nothing otherwise. */
std::optional<WalletChangeReading> answerTo(
    const std::optional<WalletChangeReading>& reading, std::uint64_t ask) {
  if (ask == NO_ACCOUNT_ASK || !reading.has_value() || reading->ask != ask) {
    return std::nullopt;
  }
  return reading;
}

}  // namespace

AccountWallet::AccountWallet(const IAccountSession& session, IAccountLink& link)
    : session_(&session), link_(&link) {}

std::optional<network::Wallet> AccountWallet::wallet() const {
  const std::optional<WalletReading> read = reading();
  return read.has_value() ? read->wallet : std::nullopt;
}

std::optional<ShipLocker> AccountWallet::ownedLooks() const {
  const std::optional<WalletReading> read = reading();
  if (!read.has_value() || !read->looks.has_value()) {
    return std::nullopt;
  }
  return lockerOf(*read->looks);
}

void AccountWallet::askWallet() {
  if (session_->signedIn()) {
    link_->askWallet(session_->token());
  }
}

std::uint64_t AccountWallet::buy(std::size_t item) {
  if (!session_->signedIn()) {
    return NO_ACCOUNT_ASK;
  }
  return link_->askPurchase(session_->token(),
                            identifiers::SHIP_LOOK_ITEM_NAMES.at(item));
}

std::optional<WalletChangeReading> AccountWallet::purchase(
    std::uint64_t ask) const {
  return answerTo(link_->purchase(), ask);
}

std::uint64_t AccountWallet::exchange(const network::ExchangeRequest& request) {
  if (!session_->signedIn()) {
    return NO_ACCOUNT_ASK;
  }
  return link_->askExchange(session_->token(), request);
}

std::optional<WalletChangeReading> AccountWallet::exchanged(
    std::uint64_t ask) const {
  return answerTo(link_->exchange(), ask);
}

void AccountWallet::followSession() {
  std::string token = session_->token();
  if (token == followedToken_) {
    return;
  }
  followedToken_ = std::move(token);
  if (!followedToken_.empty()) {
    link_->askWallet(followedToken_);
  }
}

std::optional<WalletReading> AccountWallet::reading() const {
  if (!session_->signedIn()) {
    return std::nullopt;
  }
  std::optional<WalletReading> read = link_->wallet();
  if (!read.has_value() || read->token != session_->token()) {
    return std::nullopt;
  }
  return read;
}

}  // namespace rtype::client
