#include "EntityAppearancesFixtures.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <string_view>
#include "BackgroundConstants.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "EntitySheet.hpp"
#include "EntityTypesTestConstants.hpp"
#include "HurtBox.hpp"
#include "SheetTextFixtures.hpp"
#include "ShippedEntitySheets.hpp"
#include "SpriteFrame.hpp"

using rtype::client::EntityAppearances;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::sheetText;
using rtype::game::testing::shippedSheetFolder;

EntityAppearances appearancesReading(std::string_view text) {
  const std::array sheets{sheetText(READ_TYPE_NAME, text)};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances threeAppearancesOnTwoSheets() {
  const std::array sheets{sheetText(READ_TYPE_NAME, READ_SHEET_TEXT),
                          sheetText(UNREAD_TYPE_NAME, OTHER_SHEET_TEXT),
                          sheetText(DRIFTER_SHEET, READ_SHEET_TEXT)};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances shippedAppearances() {
  return EntityAppearances::fromFolder(shippedSheetFolder());
}

bool hurtsAllTheTime(const rtype::game::EntitySheet& sheet) {
  if (!sheet.hurtZone.has_value()) {
    return false;
  }
  return std::ranges::any_of(sheet.hurtZone->boxes,
                             [](const rtype::game::HurtBox& box) noexcept {
                               return !box.activeIn.has_value();
                             });
}

bool holds(rtype::game::SpriteFrame frame,
           const rtype::game::CollisionBox& box) {
  return box.width <= static_cast<float>(frame.width) *
                          rtype::client::BACKGROUND_PIXEL_SIZE &&
         box.height <= static_cast<float>(frame.height) *
                           rtype::client::BACKGROUND_PIXEL_SIZE;
}

bool hitsAnOffsetBox(const rtype::game::EntitySheet& sheet) {
  return sheet.hitbox.offsetX != 0.0F || sheet.hitbox.offsetY != 0.0F;
}

bool holdsTheHitbox(rtype::game::SpriteFrame frame,
                    const rtype::game::CollisionBox& box) {
  return holds(frame, rtype::game::HurtBox{.offsetX = box.offsetX,
                                           .offsetY = box.offsetY,
                                           .width = box.width,
                                           .height = box.height,
                                           .activeIn = std::nullopt});
}

bool holds(rtype::game::SpriteFrame frame, const rtype::game::HurtBox& box) {
  const float halfWidth = static_cast<float>(frame.width) *
                          rtype::client::BACKGROUND_PIXEL_SIZE *
                          rtype::engine::HALF;
  const float halfHeight = static_cast<float>(frame.height) *
                           rtype::client::BACKGROUND_PIXEL_SIZE *
                           rtype::engine::HALF;
  return std::abs(box.offsetX) + (box.width * rtype::engine::HALF) <=
             halfWidth &&
         std::abs(box.offsetY) + (box.height * rtype::engine::HALF) <=
             halfHeight;
}
