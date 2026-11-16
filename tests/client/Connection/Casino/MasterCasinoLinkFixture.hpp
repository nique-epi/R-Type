#pragma once

#include <gtest/gtest.h>
#include <functional>
#include <optional>
#include "CasinoReadings.hpp"
#include "CasinoRoomState.hpp"
#include "FakeAccountMaster.hpp"
#include "MasterCasinoLink.hpp"
#include "RouletteBets.hpp"

namespace rtype::client::testing {

/** @brief A stand-in for the routes of the casino of the master, and a
 * started link to it, on its own thread. */
class MasterCasinoLinkFixture : public ::testing::Test {
 protected:
  MasterCasinoLinkFixture();

  /** @brief Stops the link before the master it reads goes away. */
  void TearDown() override;

  [[nodiscard]] FakeAccountMaster& master();
  [[nodiscard]] MasterCasinoLink& link();

  /** @returns The readings of the link once @p accepted takes them, within
   * CASINO_LINK_DEADLINE; nothing otherwise. */
  [[nodiscard]] std::optional<CasinoReadings> readingsSoon(
      const std::function<bool(const CasinoReadings&)>& accepted) const;

  /** @returns The state of the room of the tests, its bets open. */
  [[nodiscard]] static network::CasinoRoomState linkRoomState();
  /** @returns TEN_ON_RED alone. */
  [[nodiscard]] static network::RouletteBets tenOnRed();

 private:
  FakeAccountMaster master_;
  MasterCasinoLink link_;
};

}  // namespace rtype::client::testing
