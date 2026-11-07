#include "BossBarFixtures.hpp"
#include <cstdint>
#include <optional>
#include "BossBar.hpp"
#include "BossBarLook.hpp"
#include "BossBarTestConstants.hpp"
#include "BossFight.hpp"
#include "BossStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"

rtype::client::BossFight fightWith(rtype::network::BossStage stage,
                                   std::uint16_t health) {
  return rtype::client::BossFight{.stage = stage,
                                  .bossEntityId = BAR_BOSS_ID,
                                  .health = health,
                                  .fullHealth = BAR_FULL_HEALTH,
                                  .lagHealth = health,
                                  .fightingSince = FIGHT_START,
                                  .lastLossAt = std::nullopt,
                                  .phaseOneEmptiedAt = std::nullopt};
}

InterfaceCanvasRecord barPaintingOf(const rtype::client::BossBarLook& look) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintBossBar(canvas, look);
  return record;
}
