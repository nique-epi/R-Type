#pragma once

#include "BossBarLook.hpp"
#include "BossFight.hpp"
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @returns The bar at @p now: nothing before a boss fights or once it is
 * defeated. Each gauge holds half the full health, phase II the lower half;
 * the health shown grows from 0 to the health over BOSS_BAR_FILL_TIME from
 * the first status of the fight; the points lost show as a lag for
 * BOSS_LAG_TIME after the newest loss; gauge I is Spent once the health is at
 * half or less, and shatters for BOSS_GAUGE_SHATTER_TIME from then; gauge II
 * is Enraged in the rage, where the hot pixels change every
 * BOSS_BAR_FLICKER_STEP, but never with FlashPreference::Reduced.
 */
[[nodiscard]] BossBarLook bossBarAt(const BossFight& fight,
                                    engine::Duration now,
                                    FlashPreference preference);

/**
 * @brief Paints the bar of BossBarConstants.hpp when it shows, lowered by as
 * many rows as its name grew above the size the fonts are drawn at, so the
 * name stays on screen: BYDO TYRANT
 * above it in the regular font, outlined; each gauge with its border, its
 * empty inside, its lag then its fill, or hatched once spent; the hot pixels
 * of the rage; the shards of gauge I; I and II under the gauges in small
 * print.
 */
void paintBossBar(IInterfaceCanvas& canvas, const BossBarLook& look);

}  // namespace rtype::client
