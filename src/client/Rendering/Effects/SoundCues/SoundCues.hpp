#pragma once

#include "Caption.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Shows on screen the sounds that matter, for a player who cannot hear
 * them: SHOT_CUE_TEXT over the Bydo that fires, which it follows, and the
 * number of a pilot hit then HIT_CUE_TEXT over its ship. Each cue is a Caption
 * of the Popups layer outlined in INK, its line CUE_GAP pixels above the top of
 * the sprite it is over, and a Lifetime. EffectSpawner calls it only while the
 * player turned SOUND CUES on.
 */
class SoundCues {
 public:
  explicit SoundCues(EffectTools tools);

  /** @brief Shows SHOT_CUE_TEXT, in WARNING_ORANGE, over the Bydo nearest the
   * right of a missile of an enemy appearing at @p missile, for SHOT_CUE_LIFE,
   * following it; nothing without such a Bydo. */
  void showEnemyShot(game::Position missile) const;

  /** @brief Shows the number of the pilot of @p ship then HIT_CUE_TEXT, in the
   * light shade of that pilot, over the ship, for HIT_CUE_LIFE; HIT_CUE_TEXT
   * alone, in UNKNOWN_PILOT_LIGHT, for a ship of no known pilot. Call it before
   * the ship is destroyed; the cue stays where the ship stood, since the ship
   * is destroyed right after and has nothing left to follow. */
  void showPilotHit(engine::Entity ship) const;

 private:
  /** @returns A new entity writing @p caption at @p place for @p life, for
   * the caller to anchor when its cue follows its source. */
  [[nodiscard]] engine::Entity addCue(Caption caption, game::Position place,
                                      engine::Duration life) const;

  EffectTools tools_;
};

}  // namespace rtype::client
