#include "WorldContactPartsFixtures.hpp"
#include <optional>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "EntitySheet.hpp"
#include "EntityTypesTestConstants.hpp"
#include "HurtBox.hpp"
#include "HurtZone.hpp"
#include "ShippedEntitySheets.hpp"
#include "WorldContactPartsTestConstants.hpp"

namespace rtype::game::testing {

HurtZone someHurtZone() {
  return HurtZone{
      .boxes = {HurtBox{.offsetX = ALWAYS_BOX_OFFSET_X,
                        .offsetY = 0.0F,
                        .width = ALWAYS_BOX_WIDTH,
                        .height = ALWAYS_BOX_HEIGHT,
                        .activeIn = std::nullopt},
                HurtBox{.offsetX = 0.0F,
                        .offsetY = STRIKE_BOX_OFFSET_Y,
                        .width = STRIKE_BOX_WIDTH,
                        .height = STRIKE_BOX_HEIGHT,
                        .activeIn = ActionKey{.attack = STRIKE_BOX_ATTACK,
                                              .phase = EnemyPhase::Strike}}},
      .beams = {}};
}

EntitySheet drifterWith(const std::optional<HurtZone>& zone, bool survives) {
  EntitySheet sheet = shippedEntitySheets().named(DRIFTER_SHEET);
  sheet.hurtZone = zone;
  sheet.survivesContact = survives;
  return sheet;
}

}  // namespace rtype::game::testing
