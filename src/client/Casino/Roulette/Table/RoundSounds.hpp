#pragma once

#include <optional>
#include "ISoundPlayer.hpp"
#include "RouletteReading.hpp"
#include "RoundBeat.hpp"

namespace rtype::client {

/**
 * @brief The sounds of a round, played as the table sees them happen:
 * ROULETTE_SPIN_SOUND_ID, the ball thrown and rolling, as the bets close and
 * the wheel starts to spin; ROULETTE_DROP_SOUND_ID, the ball bouncing into
 * its pocket, as it drops there (ballDropped()); ROULETTE_JACKPOT_SOUND_ID
 * as the result comes with a big win (bigWinOf()). Each plays only on a
 * change seen from one frame to the next of the same round, so a player who
 * opens the table while the wheel spins hears none until the next round.
 * With MotionPreference::Reduced the wheel stays still until the result, and
 * plays neither sound of the ball; the jackpot still plays.
 */
class RoundSounds {
 public:
  /** @param sounds Plays the sounds; it must outlive the wheel. */
  explicit RoundSounds(ISoundPlayer& sounds);

  /** @brief Plays the sound of what changed in the round since the frame
   * before, as @p reading shows it. */
  void follow(const RouletteReading& reading);

 private:
  ISoundPlayer* sounds_;
  std::optional<RoundBeat> last_;
};

}  // namespace rtype::client
