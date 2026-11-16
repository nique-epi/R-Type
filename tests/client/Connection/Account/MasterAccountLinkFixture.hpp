#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <functional>
#include <optional>
#include "FakeAccountMaster.hpp"
#include "MasterAccountLink.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

namespace rtype::client::testing {

/** @brief A stand-in for the routes of the accounts of the master, and a
 * started link to it, on its own thread. */
class MasterAccountLinkFixture : public ::testing::Test {
 protected:
  MasterAccountLinkFixture();

  /** @brief Stops the link before the master it reads goes away. */
  void TearDown() override;

  [[nodiscard]] FakeAccountMaster& master();
  [[nodiscard]] MasterAccountLink& link();

  /** @returns The account read for the ask @p ask, once the link published
   * it within LINK_READING_DEADLINE; nothing otherwise. */
  [[nodiscard]] std::optional<UserReading> userSoon(std::uint64_t ask) const;

  /** @returns The wallet the link read for the ask @p ask, as soon as one
   * holds balances within LINK_READING_DEADLINE; nothing otherwise. */
  [[nodiscard]] std::optional<WalletReading> walletSoon(
      std::uint64_t ask) const;

  /** @returns The answer @p read gives to the ask @p ask, once it gave one
   * within LINK_READING_DEADLINE; nothing otherwise. */
  [[nodiscard]] static std::optional<WalletChangeReading> answerSoon(
      const std::function<std::optional<WalletChangeReading>()>& read,
      std::uint64_t ask);

 private:
  FakeAccountMaster master_;
  MasterAccountLink link_;
};

}  // namespace rtype::client::testing
