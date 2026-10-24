#pragma once

#include "IInterfaceCanvas.hpp"
#include "IntroFrame.hpp"

namespace rtype::client {

/** @brief Paints every missile of an enemy in flight, left behind by the
 * boost. */
void paintEnemyMissiles(IInterfaceCanvas& canvas, const IntroFrame& frame);

/** @brief Paints every missile of a player: in flight, its smoke when it
 * swerves, its head and, just after the shot, the muzzle flash; once it hit,
 * its impact, left behind by the boost. */
void paintPlayerMissiles(IInterfaceCanvas& canvas, const IntroFrame& frame);

/** @brief Paints every burst: its explosions of the effect sheet, its sparks
 * and its drops, all left behind by the boost. The sparks go through
 * SPARK_RAMP after its flash, as in the mockup; with REDUCE FLASHES, the
 * explosions show their version without flash and the drops lose the first
 * color of BYDO_RAMP, as the drops of a game do. */
void paintBursts(IInterfaceCanvas& canvas, const IntroFrame& frame);

/** @brief Paints every ring, growing fast then slowly and fading through
 * BIG_CATCH_RING_RAMP, without its white with REDUCE FLASHES, left behind by
 * the boost. */
void paintRings(IInterfaceCanvas& canvas, const IntroFrame& frame);

}  // namespace rtype::client
