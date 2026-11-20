#include "WheelSounds.hpp"
#include <optional>
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "RoulettePhase.hpp"
#include "RouletteReading.hpp"
#include "RouletteTexts.hpp"
#include "SoundConstants.hpp"
#include "WheelBeat.hpp"
#include "WheelMotion.hpp"

namespace rtype::client {

namespace {

/** @returns Where the wheel of @p reading stands; nothing without a room, or
 * with less motion, when the wheel does not move. */
std::optional<WheelBeat> beatOf(const RouletteReading& reading) {
  if (!reading.room.has_value() ||
      reading.motion == MotionPreference::Reduced) {
    return std::nullopt;
  }
  const network::RoulettePhase phase = reading.room->phase;
  return WheelBeat{.round = reading.room->round,
                   .phase = phase,
                   .ballInPocket = phase == network::RoulettePhase::Result ||
                                   (phase == network::RoulettePhase::Spinning &&
                                    ballDropped(phaseTimeLeft(reading)))};
}

/** @returns Whether the wheel started to spin from @p before to @p now. */
bool startedToSpin(const WheelBeat& before, const WheelBeat& now) {
  return before.phase == network::RoulettePhase::Betting &&
         now.phase == network::RoulettePhase::Spinning;
}

/** @returns Whether the ball dropped in its pocket from @p before to
 * @p now. */
bool ballFell(const WheelBeat& before, const WheelBeat& now) {
  return before.phase == network::RoulettePhase::Spinning &&
         !before.ballInPocket && now.ballInPocket;
}

}  // namespace

WheelSounds::WheelSounds(ISoundPlayer& sounds) : sounds_(&sounds) {}

void WheelSounds::follow(const RouletteReading& reading) {
  const std::optional<WheelBeat> now = beatOf(reading);
  if (now.has_value() && last_.has_value() && last_->round == now->round) {
    if (startedToSpin(*last_, *now)) {
      sounds_->play(ROULETTE_SPIN_SOUND_ID);
    }
    if (ballFell(*last_, *now)) {
      sounds_->play(ROULETTE_DROP_SOUND_ID);
    }
  }
  last_ = now;
}

}  // namespace rtype::client
