#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include "BetsReading.hpp"
#include "CasinoChannel.hpp"
#include "CasinoRoomState.hpp"
#include "FakeAccountSession.hpp"
#include "FakeWallet.hpp"
#include "RecordingCasinoLink.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A casino channel over a recording link, the player signed in with
 * CHANNEL_TOKEN; the clock of the client moves only when a test lets it. */
class CasinoChannelFixture : public ::testing::Test {
 protected:
  CasinoChannelFixture();

  [[nodiscard]] RecordingCasinoLink& link();
  [[nodiscard]] FakeAccountSession& session();
  [[nodiscard]] FakeWallet& wallet();
  [[nodiscard]] CasinoChannel& channel();

  /** @brief Lets @p elapsed pass in one frame. */
  void wait(engine::Duration elapsed);

  /** @brief Has the link answer the newest read of the room with @p state. */
  void answerTheRead(const network::CasinoRoomState& state);

  /** @returns CHANNEL_ROOM at CHANNEL_ROUND, its bets open, as the master
   * answers it at @p masterTime. */
  [[nodiscard]] static network::CasinoRoomState roomAt(
      std::chrono::milliseconds masterTime);

  /** @returns The answer to the first bets, TEN_ON_BLACK on CHANNEL_ROUND,
   * taken, CHANNEL_CHIPS_LEFT left in the wallet. */
  [[nodiscard]] static BetsReading tenOnBlackTaken();

 private:
  RecordingCasinoLink link_;
  FakeAccountSession session_;
  FakeWallet wallet_;
  CasinoChannel channel_;
};

}  // namespace rtype::client::testing
