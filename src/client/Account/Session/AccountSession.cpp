#include "AccountSession.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "AccountLinkConstants.hpp"
#include "IAccountLink.hpp"
#include "ProtocolConstants.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"

namespace rtype::client {

AccountSession::AccountSession(IAccountLink& link)
    : link_(&link), ask_(NO_ACCOUNT_ASK) {}

bool AccountSession::signedIn() const {
  const std::optional<UserReading> reading = answer();
  return reading.has_value() && reading->user.has_value();
}

std::uint64_t AccountSession::accountId() const {
  const std::optional<UserReading> reading = answer();
  if (!reading.has_value() || !reading->user.has_value()) {
    return network::GUEST_ACCOUNT_ID;
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
}

std::optional<UserReading> AccountSession::answer() const {
  if (ask_ == NO_ACCOUNT_ASK) {
    return std::nullopt;
  }
  const std::optional<UserReading> reading = link_->user();
  if (!reading.has_value() || reading->ask != ask_) {
    return std::nullopt;
  }
  return reading;
}

}  // namespace rtype::client
