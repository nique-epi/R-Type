#include "WorldContactPartsFixtures.hpp"
#include <cstdint>
#include <optional>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "HurtBox.hpp"
#include "HurtZone.hpp"

namespace rtype::game::testing {

namespace {

constexpr float ALWAYS_BOX_OFFSET_X = 48.0F;
constexpr float ALWAYS_BOX_WIDTH = 64.0F;
constexpr float ALWAYS_BOX_HEIGHT = 32.0F;
constexpr float STRIKE_BOX_OFFSET_Y = 16.0F;
constexpr float STRIKE_BOX_WIDTH = 40.0F;
constexpr float STRIKE_BOX_HEIGHT = 24.0F;
constexpr std::uint8_t STRIKE_BOX_ATTACK = 3;

}  // namespace

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
                                              .phase = EnemyPhase::Strike}}}};
}

}  // namespace rtype::game::testing
