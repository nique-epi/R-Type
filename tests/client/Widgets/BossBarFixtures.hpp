#pragma once

#include <cstdint>
#include "BossBarLook.hpp"
#include "BossFight.hpp"
#include "BossStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns The fight of the boss of the bar tests from FIGHT_START, in
 * @p stage with @p health of its BAR_FULL_HEALTH points, no loss shown. */
[[nodiscard]] rtype::client::BossFight fightWith(
    rtype::network::BossStage stage, std::uint16_t health);

/** @returns What paintBossBar() paints for @p look. */
[[nodiscard]] InterfaceCanvasRecord barPaintingOf(
    const rtype::client::BossBarLook& look);
