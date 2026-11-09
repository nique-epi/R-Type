#include "DelayedInputs.hpp"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include "AppliedInput.hpp"
#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"

namespace rtype::server {

namespace {

/** @returns The seat of the player numbered @p playerNumber, from 0; nothing
 * for a number no seat has. */
std::optional<std::size_t> seatOf(std::uint8_t playerNumber) {
  if (playerNumber < network::FIRST_PLAYER_NUMBER ||
      playerNumber >= network::FIRST_PLAYER_NUMBER + network::MAXIMUM_PLAYERS) {
    return std::nullopt;
  }
  return static_cast<std::size_t>(playerNumber - network::FIRST_PLAYER_NUMBER);
}

/**
 * @brief Counts in the pace of the player of @p seat a tick that releases
 * @p count of its inputs: none is a tick without input, two a catch-up. A
 * player whose first input was not applied yet counts nothing.
 */
void notePace(PlayerRoster& roster, std::size_t seat, std::size_t count) {
  ConnectedPlayer* const player = roster.findByNumber(
      static_cast<std::uint8_t>(network::FIRST_PLAYER_NUMBER + seat));
  if (player == nullptr || !player->appliedInput.has_value()) {
    return;
  }
  if (count == 0) {
    ++player->inputPace.ticksWithoutInput;
  } else if (count > 1) {
    ++player->inputPace.catchUps;
  }
}

}  // namespace

DelayedInputs::DelayedInputs(std::size_t delayTicks) : delayTicks_(delayTicks) {
  if (delayTicks < LOWEST_INPUT_DELAY_TICKS ||
      delayTicks > HIGHEST_INPUT_DELAY_TICKS) {
    throw InvalidInputDelayException(delayTicks);
  }
}

void DelayedInputs::push(std::uint8_t playerNumber,
                         const network::PlayerInput& input) {
  const std::optional<std::size_t> seat = seatOf(playerNumber);
  if (!seat.has_value()) {
    return;
  }
  held_.at(*seat).push_back(HeldInput{.playerNumber = playerNumber,
                                      .input = input,
                                      .dueTick = currentTick_ + delayTicks_});
}

void DelayedInputs::release(PlayerRoster& roster, const InputSink& sink) {
  for (std::size_t seat = 0; seat < held_.size(); ++seat) {
    std::deque<HeldInput>& held = held_.at(seat);
    const std::size_t due = dueCount(held);
    notePace(roster, seat, due);
    for (std::size_t count = due; count > 0; --count) {
      const HeldInput next = held.front();
      held.pop_front();
      ConnectedPlayer* const player = roster.findByNumber(next.playerNumber);
      if (player != nullptr) {
        sink(*player, next.input);
        player->appliedInput =
            AppliedInput{.inputNumber = next.input.inputNumber,
                         .shownTick = next.input.shownTick};
      }
    }
  }
  ++currentTick_;
}

void DelayedInputs::clear(std::uint8_t playerNumber) {
  if (const std::optional<std::size_t> seat = seatOf(playerNumber)) {
    held_.at(*seat).clear();
  }
}

std::size_t DelayedInputs::dueCount(const std::deque<HeldInput>& held) const {
  if (held.empty() || held.front().dueTick > currentTick_) {
    return 0;
  }
  const bool lagging =
      currentTick_ - held.front().dueTick > INPUT_CATCH_UP_TICKS;
  return lagging && held.size() > 1 && held.at(1).dueTick <= currentTick_ ? 2
                                                                          : 1;
}

}  // namespace rtype::server
