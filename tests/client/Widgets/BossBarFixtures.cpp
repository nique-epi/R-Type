#include "BossBarFixtures.hpp"
#include <cstdint>
#include <optional>
#include "BossBar.hpp"
#include "BossBarLook.hpp"
#include "BossBarTestConstants.hpp"
#include "BossFight.hpp"
#include "BossStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextMetrics.hpp"

rtype::client::BossFight fightWith(rtype::network::BossStage stage,
                                   std::uint16_t health) {
  return rtype::client::BossFight{.stage = stage,
                                  .health = health,
                                  .fullHealth = BAR_FULL_HEALTH,
                                  .lagHealth = health,
                                  .fightingSince = FIGHT_START,
                                  .lastLossAt = std::nullopt,
                                  .phaseOneEmptiedAt = std::nullopt};
}

InterfaceCanvasRecord barPaintingOf(const rtype::client::BossBarLook& look,
                                    rtype::client::TextMetrics metrics) {
  InterfaceCanvasRecord record;
  record.metrics = metrics;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintBossBar(canvas, look);
  return record;
}
