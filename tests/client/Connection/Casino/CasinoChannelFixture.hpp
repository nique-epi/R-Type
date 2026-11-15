#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ArrivalTally.hpp"
#include "CasinoChannel.hpp"
#include "CasinoPlace.hpp"
#include "ContentLoading.hpp"
#include "ContentPackCache.hpp"
#include "GameLoadedReminder.hpp"
#include "MessageDispatcher.hpp"
#include "RecordingDatagramLink.hpp"
#include "RecordingWaitingRoom.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "ServerSession.hpp"
#include "ServerSessionParts.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A session and the casino that follows it, wired as PlayerConnection
 * wires them, over a recording link and a recording waiting room. */
class CasinoChannelFixture : public ::testing::Test {
 protected:
  /** @brief Connects, then has the server accept the attempt, its acceptance
   * carrying SESSION_TOKEN and ACCEPTED_PLAYER_NUMBER. */
  void connectAndGetAccepted();

  /** @brief Runs one frame of the network stage. */
  void update(engine::Duration elapsed = engine::Duration::zero());

  /** @brief The server sends @p round in a datagram of the next sequence,
   * then a frame goes by. */
  void serverSends(const network::RouletteRound& round);

  /** @returns Round @p number in @p phase, listing the player with
   * STAKE_OF_A_HUNDRED and, once drawn, PAYOUT_OF_A_HUNDRED. */
  [[nodiscard]] static network::RouletteRound roundListingThePlayer(
      std::uint16_t number, network::RoulettePhase phase);

  [[nodiscard]] CasinoChannel& casino();
  [[nodiscard]] RecordingWaitingRoom& room();

  /** @returns The places sent to the server, copies included. */
  [[nodiscard]] std::vector<network::CasinoPlace> sentPlaces() const;
  /** @returns The bets sent to the server, copies included. */
  [[nodiscard]] std::vector<network::RouletteBets> sentBets() const;

 private:
  RecordingDatagramLink link_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  RecordingWaitingRoom room_;
  CasinoChannel casino_{dispatcher_, room_};
  SessionEntityTypes entityTypes_{testLocalContent()};
  ContentLoading content_{ContentPackCache{unusedCacheFolder()}};
  ArrivalTally arrivals_;
  ServerSession session_{
      ServerSessionParts{.link = link_,
                         .dispatcher = dispatcher_,
                         .gameLoaded = gameLoaded_,
                         .entityTypes = entityTypes_,
                         .content = content_,
                         .arrivals = arrivals_,
                         .observers = {gameLoaded_, casino_}}};
  std::uint16_t nextSequence_ = FIRST_SEQUENCE;
};

}  // namespace rtype::client::testing
