#include "RoundSounds.hpp"
#include <optional>
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "RoulettePhase.hpp"
#include "RouletteReading.hpp"
#include "RouletteScene.hpp"
#include "RouletteTexts.hpp"
#include "RoundBeat.hpp"
#include "SoundConstants.hpp"
#include "WheelMotion.hpp"

namespace rtype::client {

namespace {

/** @returns Where the round of @p reading stands; nothing without a
 * room. */
std::optional<RoundBeat> beatOf(const RouletteReading& reading) {
  if (!reading.room.has_value()) {
    return std::nullopt;
  }
  const network::RoulettePhase phase = reading.room->phase;
  return RoundBeat{.round = reading.room->round,
                   .phase = phase,
                   .ballInPocket = phase == network::RoulettePhase::Result ||
                                   (phase == network::RoulettePhase::Spinning &&
                                    ballDropped(phaseTimeLeft(reading)))};
}

/** @returns Whether the wheel started to spin from @p before to @p now. */
bool startedToSpin(const RoundBeat& before, const RoundBeat& now) {
  return before.phase == network::RoulettePhase::Betting &&
         now.phase == network::RoulettePhase::Spinning;
}

/** @returns Whether the ball dropped in its pocket from @p before to
 * @p now. */
bool ballFell(const RoundBeat& before, const RoundBeat& now) {
  return before.phase == network::RoulettePhase::Spinning &&
         !before.ballInPocket && now.ballInPocket;
}

/** @returns Whether the result of the round came from @p before to
 * @p now. */
bool resultCame(const RoundBeat& before, const RoundBeat& now) {
  return before.phase != network::RoulettePhase::Result &&
         now.phase == network::RoulettePhase::Result;
}

}  // namespace

RoundSounds::RoundSounds(ISoundPlayer& sounds) : sounds_(&sounds) {}

void RoundSounds::follow(const RouletteReading& reading) {
  const std::optional<RoundBeat> now = beatOf(reading);
  if (now.has_value() && last_.has_value() && last_->round == now->round) {
    const bool wheelMoves = reading.motion != MotionPreference::Reduced;
    if (wheelMoves && startedToSpin(*last_, *now)) {
      sounds_->play(ROULETTE_SPIN_SOUND_ID);
    }
    if (wheelMoves && ballFell(*last_, *now)) {
      sounds_->play(ROULETTE_DROP_SOUND_ID);
    }
    if (resultCame(*last_, *now) && bigWinOf(reading).has_value()) {
      sounds_->play(ROULETTE_JACKPOT_SOUND_ID);
    }
  }
  last_ = now;
}

}  // namespace rtype::client
