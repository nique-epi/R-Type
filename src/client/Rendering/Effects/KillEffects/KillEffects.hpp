#pragma once

#include "DownedShip.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "KillChain.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class GainFigures;

/**
 * @brief Plays the destructions: a Bydo destroyed (T2), every fifth kill of a
 * chain (T3, see KillChain), a ship destroyed (T4).
 *
 * A destruction freezes its victim, never the game: the first frame of its
 * explosion stays a few frames longer, its animation starting at minus that
 * hold, and what follows (drops, debris, shockwave) waits as long with a
 * negative age (see PixelEffects). Every method but advance() adds
 * components, so none may be called during a forEach of the registry. The
 * gains must outlive it.
 */
class KillEffects {
 public:
  KillEffects(EffectTools tools, GainFigures& gains);

  /** @brief Moves the time since the last kill on. */
  void advance(engine::Duration elapsed);

  /**
   * @brief Plays T2 for a Bydo destroyed, centered where it died, or T3 when
   * it is a big catch: its explosion holds its first frame KILL_HOLD or
   * BIG_CATCH_HOLD longer while it vibrates, then its drops burst, and at T3
   * a shockwave opens and the camera shakes. The kill waits for the gain of
   * score that makes its figure (see GainFigures).
   */
  void playKill(game::Position center);

  /** @brief Plays the three fireballs of a ship, alone: playShipDown() plays
   * the whole of T4. */
  void playShipExplosion(game::Position center);

  /**
   * @brief Plays T4 for a ship destroyed: its picture stays white, or
   * shaded, SHIP_DOWN_HOLD while it vibrates, then its three fireballs, its
   * debris and a shockwave in the colors of its pilot (see PilotShip), and
   * the camera shakes. Call it before the ship is destroyed. A ship without a
   * Position or a Sprite plays nothing.
   */
  void playShipDown(engine::Entity ship);

 private:
  /** @brief Leaves the picture of a ship destroyed, white or shaded, in the
   * Players layer, for SHIP_DOWN_HOLD. */
  void freeze(const DownedShip& ship) const;

  EffectTools tools_;
  GainFigures* gains_;
  KillChain killChain_;
};

}  // namespace rtype::client
