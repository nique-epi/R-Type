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
  for (std::deque<HeldInput>& held : held_) {
    for (std::size_t count = dueCount(held); count > 0; --count) {
      const HeldInput due = held.front();
      held.pop_front();
      ConnectedPlayer* const player = roster.findByNumber(due.playerNumber);
      if (player != nullptr) {
        sink(*player, due.input);
        player->appliedInput =
            AppliedInput{.inputNumber = due.input.inputNumber,
                         .shownTick = due.input.shownTick};
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
