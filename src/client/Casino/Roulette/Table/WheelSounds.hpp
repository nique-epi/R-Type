#pragma once

#include <optional>
#include "ISoundPlayer.hpp"
#include "RouletteReading.hpp"
#include "WheelBeat.hpp"

namespace rtype::client {

/**
 * @brief The sounds of the wheel, played as the table sees them happen:
 * ROULETTE_SPIN_SOUND_ID, the ball thrown and rolling, as the bets close and
 * the wheel starts to spin, and ROULETTE_DROP_SOUND_ID, the ball bouncing
 * into its pocket, as it drops there (ballDropped()). Each plays only on a
 * change seen from one frame to the next of the same round, so a player who
 * opens the table while the wheel spins hears neither until the next round.
 * With MotionPreference::Reduced the wheel stays still until the result, and
 * plays neither.
 */
class WheelSounds {
 public:
  /** @param sounds Plays the sounds; it must outlive the wheel. */
  explicit WheelSounds(ISoundPlayer& sounds);

  /** @brief Plays the sound of what changed on the wheel since the frame
   * before, as @p reading shows it. */
  void follow(const RouletteReading& reading);

 private:
  ISoundPlayer* sounds_;
  std::optional<WheelBeat> last_;
};

}  // namespace rtype::client
