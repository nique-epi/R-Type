#include "CasinoRoomWatch.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <tuple>
#include "CasinoChannelConstants.hpp"
#include "CasinoRoomReading.hpp"
#include "CasinoRoomState.hpp"
#include "ICasinoLink.hpp"
#include "RoulettePhase.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

using MillisecondDate = std::chrono::sys_time<std::chrono::milliseconds>;

/** @returns The phase the dates of @p state give at @p now: the bets open
 * until closesAt, the wheel turning until resultAt, the result after. */
network::RoulettePhase phaseAt(const network::CasinoRoomState& state,
                               MillisecondDate now) {
  if (now < state.closesAt) {
    return network::RoulettePhase::Betting;
  }
  if (now < state.resultAt) {
    return network::RoulettePhase::Spinning;
  }
  return network::RoulettePhase::Result;
}

/** @returns Whether @p read is a later look at the room than @p kept: a
 * later round, or the same round at a later time of the master. */
bool isNewer(const network::CasinoRoomState& read,
             const network::CasinoRoomState& kept) {
  return std::tie(read.round, read.serverTime) >
         std::tie(kept.round, kept.serverTime);
}

}  // namespace

CasinoRoomWatch::CasinoRoomWatch(ICasinoLink& link) : link_(&link) {}

void CasinoRoomWatch::watch(std::uint8_t room) {
  if (room_ == room) {
    return;
  }
  room_ = room;
  state_.reset();
  ask_ = link_->askRoom(room);
  answered_ = false;
}

void CasinoRoomWatch::stop() {
  room_.reset();
  state_.reset();
  answered_ = true;
}

void CasinoRoomWatch::follow(const std::optional<CasinoRoomReading>& reading,
                             engine::Duration elapsed) {
  if (!room_.has_value()) {
    return;
  }
  sinceState_ += elapsed;
  sinceAnswer_ += elapsed;
  keep(reading);
  if (answered_ && sinceAnswer_ >= CASINO_ROOM_POLL_INTERVAL) {
    ask_ = link_->askRoom(*room_);
    answered_ = false;
  }
}

std::optional<network::CasinoRoomState> CasinoRoomWatch::shown() const {
  if (!state_.has_value()) {
    return std::nullopt;
  }
  network::CasinoRoomState shown = *state_;
  shown.serverTime +=
      std::chrono::duration_cast<std::chrono::milliseconds>(sinceState_);
  shown.phase = phaseAt(shown, shown.serverTime);
  return shown;
}

std::optional<std::uint8_t> CasinoRoomWatch::room() const { return room_; }

void CasinoRoomWatch::keep(const std::optional<CasinoRoomReading>& reading) {
  if (!reading.has_value() || reading->ask != ask_) {
    return;
  }
  if (!answered_) {
    answered_ = true;
    sinceAnswer_ = engine::Duration::zero();
  }
  if (!reading->state.has_value() || reading->state->id != room_ ||
      (state_.has_value() && !isNewer(*reading->state, *state_))) {
    return;
  }
  state_ = reading->state;
  sinceState_ = engine::Duration::zero();
}

}  // namespace rtype::client
