#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include "MasterDatabase.hpp"
#include "SignInFailure.hpp"

namespace rtype::master {

/** @brief The table `sign_in_failures`: the only code that writes its SQL.
 * Each call runs behind the lock of the MasterDatabase. */
class SignInFailureRepository {
 public:
  explicit SignInFailureRepository(MasterDatabase& database);

  /** @returns How many sign-ins from @p address failed since @p since. */
  [[nodiscard]] std::int64_t failuresSince(const std::string& address,
                                           std::chrono::sys_seconds since);

  /** @brief Adds @p failure, and forgets in the same transaction every failure
   * before @p forgetBefore, which no longer counts. */
  void add(const SignInFailure& failure, std::chrono::sys_seconds forgetBefore);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
