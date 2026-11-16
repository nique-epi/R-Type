#pragma once

#include <gtest/gtest.h>
#include "BetsReceipt.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "FakeAccountMaster.hpp"
#include "MasterCasinoClient.hpp"
#include "RecordedRequest.hpp"
#include "RouletteBets.hpp"
#include "RoundBets.hpp"

/** @brief A stand-in for the routes of the casino of the master, and a client
 * of it. */
class MasterCasinoClientFixture : public ::testing::Test {
 protected:
  MasterCasinoClientFixture();

  [[nodiscard]] FakeAccountMaster& master();
  [[nodiscard]] rtype::network::MasterCasinoClient& client();

  /** @returns The last request the master received; an empty one before the
   * first. */
  [[nodiscard]] RecordedRequest lastRequest() const;

  /** @returns The room of the tests in the listing of the rooms. */
  [[nodiscard]] static rtype::network::CasinoRoomListing testRooms();
  /** @returns The state of the room of the tests, its bets open, ACE
   * seated. */
  [[nodiscard]] static rtype::network::CasinoRoomState testRoomState();
  /** @returns TEN_ON_17 alone. */
  [[nodiscard]] static rtype::network::RouletteBets tenOn17();
  /** @returns The receipt of tenOn17() on the round of the tests. */
  [[nodiscard]] static rtype::network::BetsReceipt tenOn17Receipt();
  /** @returns tenOn17() on the round of the tests, once 17 was drawn. */
  [[nodiscard]] static rtype::network::RoundBets tenOn17Paid();

 private:
  FakeAccountMaster master_;
  rtype::network::MasterCasinoClient client_;
};
