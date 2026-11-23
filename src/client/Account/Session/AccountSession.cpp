#include "AccountSession.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "AccountLinkConstants.hpp"
#include "IAccountLink.hpp"
#include "ISessionTokenStorage.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"

namespace rtype::client {

AccountSession::AccountSession(IAccountLink& link,
                               ISessionTokenStorage& storage)
    : link_(&link),
      storage_(&storage),
      ask_(NO_ACCOUNT_ASK),
      rememberedAsk_(NO_ACCOUNT_ASK) {}

bool AccountSession::signedIn() const {
  const std::optional<UserReading> reading = answer();
  return reading.has_value() && reading->user.has_value();
}

std::uint64_t AccountSession::accountId() const {
  const std::optional<UserReading> reading = answer();
  if (!reading.has_value() || !reading->user.has_value()) {
    return NO_ACCOUNT_ID;
  }
  return static_cast<std::uint64_t>(reading->user->id);
}

std::string AccountSession::nickname() const {
  const std::optional<UserReading> reading = answer();
  if (!reading.has_value() || !reading->user.has_value()) {
    return {};
  }
  return reading->user->nickname;
}

std::string AccountSession::token() const {
  return signedIn() ? token_ : std::string{};
}

std::optional<SignInRefusal> AccountSession::signInRefusal() const {
  const std::optional<UserReading> reading = answer();
  return reading.has_value() ? reading->refusal : std::nullopt;
}

void AccountSession::signIn(const std::string& token) {
  token_ = token;
  ask_ = link_->askUser(token);
}

void AccountSession::signOut() {
  token_.clear();
  ask_ = NO_ACCOUNT_ASK;
  storage_->forget();
  rememberedToken_.clear();
}

void AccountSession::restore() {
  const std::optional<std::string> kept = storage_->load();
  if (!kept.has_value()) {
    return;
  }
  rememberedToken_ = *kept;
  signIn(*kept);
}

void AccountSession::rememberAnswer() {
  if (ask_ == rememberedAsk_) {
    return;
  }
  const std::optional<UserReading> reading = answer();
  if (!reading.has_value()) {
    return;
  }
  rememberedAsk_ = ask_;
  if (reading->user.has_value() && token_ != rememberedToken_) {
    storage_->keep(token_);
    rememberedToken_ = token_;
  } else if (reading->refusal == SignInRefusal::TokenRefused) {
    storage_->forget();
    rememberedToken_.clear();
  }
}

std::optional<UserReading> AccountSession::answer() const {
  if (ask_ == NO_ACCOUNT_ASK) {
    return std::nullopt;
  }
  std::optional<UserReading> reading = link_->user();
  if (!reading.has_value() || reading->ask != ask_) {
    return std::nullopt;
  }
  return reading;
}

}  // namespace rtype::client
