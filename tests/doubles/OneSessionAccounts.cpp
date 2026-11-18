#include "OneSessionAccounts.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "User.hpp"

OneSessionAccounts::OneSessionAccounts(std::string token,
                                       rtype::network::User user)
    : token_(std::move(token)), user_(std::move(user)) {}

std::optional<rtype::network::User> OneSessionAccounts::userOfToken(
    std::string_view token, std::chrono::sys_seconds now) {
  lastAskedAt_ = now;
  if (token != token_) {
    return std::nullopt;
  }
  return user_;
}

std::optional<std::chrono::sys_seconds> OneSessionAccounts::lastAskedAt()
    const {
  return lastAskedAt_;
}
