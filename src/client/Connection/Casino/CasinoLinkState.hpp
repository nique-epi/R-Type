#pragma once

#include "BetsReading.hpp"
#include "CasinoAsks.hpp"
#include "CasinoRoomReading.hpp"
#include "CasinoRoomsReading.hpp"
#include "LatestValue.hpp"
#include "OwnBetsReading.hpp"
#include "SeatReading.hpp"

namespace rtype::client {

/** @brief What the frame loop and the thread of a MasterCasinoLink share:
 * the asks, newest first, and what the thread read, newest first; and the
 * asks it answered so far, which only that thread reads and writes. */
struct CasinoLinkState {
  concurrency::LatestValue<CasinoAsks> asks;
  CasinoAsks served;
  concurrency::LatestValue<CasinoRoomsReading> rooms;
  concurrency::LatestValue<CasinoRoomReading> room;
  concurrency::LatestValue<SeatReading> seat;
  concurrency::LatestValue<BetsReading> bets;
  concurrency::LatestValue<OwnBetsReading> ownBets;
};

}  // namespace rtype::client
