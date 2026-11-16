#include "MasterAccountLinkFixture.hpp"
#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <thread>
#include "FakeAccountMaster.hpp"
#include "MasterAccountLink.hpp"
#include "MasterAccountLinkTestConstants.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

namespace rtype::client::testing {

namespace {

/** @returns What @p read gives once @p accepted takes it, within
 * LINK_READING_DEADLINE; nothing otherwise. */
template <typename Reading>
std::optional<Reading> readingSoon(
    const std::function<std::optional<Reading>()>& read,
    const std::function<bool(const Reading&)>& accepted) {
  const auto deadline =
      std::chrono::steady_clock::now() + LINK_READING_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    const std::optional<Reading> reading = read();
    if (reading.has_value() && accepted(*reading)) {
      return reading;
    }
    std::this_thread::sleep_for(LINK_POLL_STEP);
  }
  return std::nullopt;
}

}  // namespace

MasterAccountLinkFixture::MasterAccountLinkFixture() {
  link_.start(master_.url());
}

void MasterAccountLinkFixture::TearDown() { link_.stop(); }

FakeAccountMaster& MasterAccountLinkFixture::master() { return master_; }

MasterAccountLink& MasterAccountLinkFixture::link() { return link_; }

std::optional<UserReading> MasterAccountLinkFixture::userSoon(
    std::uint64_t ask) const {
  return readingSoon<UserReading>(
      [this] { return link_.user(); },
      [ask](const UserReading& reading) { return reading.ask == ask; });
}

std::optional<WalletReading> MasterAccountLinkFixture::walletSoon() const {
  return readingSoon<WalletReading>(
      [this] { return link_.wallet(); },
      [](const WalletReading& reading) { return reading.wallet.has_value(); });
}

std::optional<WalletChangeReading> MasterAccountLinkFixture::answerSoon(
    const std::function<std::optional<WalletChangeReading>()>& read,
    std::uint64_t ask) {
  return readingSoon<WalletChangeReading>(
      read,
      [ask](const WalletChangeReading& reading) { return reading.ask == ask; });
}

}  // namespace rtype::client::testing
