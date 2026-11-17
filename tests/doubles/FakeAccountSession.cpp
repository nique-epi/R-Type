#include "FakeAccountSession.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "AccountLinkConstants.hpp"
#include "SignInRefusal.hpp"

void FakeAccountSession::signInAs(std::uint64_t accountId,
                                  const std::string& token) {
  accountId_ = accountId;
  token_ = token;
}

bool FakeAccountSession::signedIn() const { return accountId_.has_value(); }

std::uint64_t FakeAccountSession::accountId() const {
  return accountId_.value_or(rtype::client::NO_ACCOUNT_ID);
}

std::string FakeAccountSession::nickname() const {
  return signedIn() ? accountNickname_ : std::string{};
}

std::string FakeAccountSession::token() const {
  return signedIn() ? token_ : std::string{};
}

std::optional<rtype::client::SignInRefusal> FakeAccountSession::signInRefusal()
    const {
  return std::nullopt;
}

void FakeAccountSession::signIn(const std::string& token) {
  askedToken_ = token;
}

void FakeAccountSession::signOut() {
  accountId_.reset();
  token_.clear();
}

const std::string& FakeAccountSession::askedToken() const {
  return askedToken_;
}

void FakeAccountSession::nameAccount(const std::string& nickname) {
  accountNickname_ = nickname;
}
