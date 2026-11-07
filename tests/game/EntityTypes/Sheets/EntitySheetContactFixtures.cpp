#include "EntitySheetContactFixtures.hpp"
#include <gtest/gtest.h>
#include "EntitySheet.hpp"
#include "HurtZone.hpp"

namespace rtype::game::testing {

const HurtZone& zoneOf(const EntitySheet& sheet) {
  static const HurtZone none{};
  EXPECT_TRUE(sheet.hurtZone.has_value());
  return sheet.hurtZone.has_value() ? *sheet.hurtZone : none;
}

}  // namespace rtype::game::testing
