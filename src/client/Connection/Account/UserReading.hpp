#pragma once

#include <cstdint>
#include <optional>
#include "SignInRefusal.hpp"
#include "User.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the account of a token:
 * the account, or why there is none. */
struct UserReading {
  /** @brief The number of the ask it answers (see AccountAsks). */
  std::uint64_t ask{};
  std::optional<network::User> user;
  std::optional<SignInRefusal> refusal;
};

}  // namespace rtype::client
