#include "DelayedInputs.hpp"
#include <cstddef>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"

namespace rtype::server {

DelayedInputs::DelayedInputs(std::size_t delayTicks) : delayTicks_(delayTicks) {
  if (delayTicks < LOWEST_INPUT_DELAY_TICKS ||
      delayTicks > HIGHEST_INPUT_DELAY_TICKS) {
    throw InvalidInputDelayException(delayTicks);
  }
}

void DelayedInputs::push(std::uint8_t playerNumber,
                         const network::PlayerInput& input) {
  held_.push_back(HeldInput{.playerNumber = playerNumber,
                            .input = input,
                            .dueTick = currentTick_ + delayTicks_});
}

void DelayedInputs::release(PlayerRoster& roster, const InputSink& sink) {
  while (!held_.empty() && held_.front().dueTick <= currentTick_) {
    const HeldInput due = held_.front();
    held_.pop_front();
    ConnectedPlayer* const player = roster.findByNumber(due.playerNumber);
    if (player != nullptr) {
      sink(*player, due.input);
      player->inputApplied = true;
    }
  }
  ++currentTick_;
}

void DelayedInputs::clear(std::uint8_t playerNumber) {
  std::erase_if(held_, [playerNumber](const HeldInput& held) {
    return held.playerNumber == playerNumber;
  });
}

}  // namespace rtype::server
