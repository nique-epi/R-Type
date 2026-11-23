#include "EntityTypeLooksFixture.hpp"
#include <array>
#include <string>
#include "CollisionBox.hpp"
#include "EntityAppearances.hpp"
#include "EntityType.hpp"
#include "EntityTypeLooks.hpp"
#include "EntityTypeLooksTestConstants.hpp"
#include "HurtBeam.hpp"
#include "HurtZone.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityType.hpp"
#include "SheetTextFixtures.hpp"
#include "ShipArsenal.hpp"

namespace rtype::client::testing {

namespace {

/** @returns The appearance of LOOKED_UP_TYPE_NAME alone. */
EntityAppearances lookedUpAppearance() {
  const std::array sheets{
      game::testing::sheetText(LOOKED_UP_TYPE_NAME, LOOKED_UP_SHEET_TEXT)};
  return EntityAppearances::fromTexts(sheets);
}

/** @returns The client's own content of these tests: LOOKED_UP_TYPE_NAME,
 * with its box and its hurt beam, then TYPE_WITHOUT_APPEARANCE_NAME, both
 * enemies, and the gun LOOKED_UP_SHIP_GUN. */
LocalContent twoEnemies() {
  return LocalContent{
      .fingerprint = {},
      .types =
          {game::NumberedEntityType{
               .name = std::string{LOOKED_UP_TYPE_NAME},
               .side = game::EntityType::Enemy,
               .hitbox = game::CollisionBox{.width = LOOKED_UP_BOX_WIDTH,
                                            .height = LOOKED_UP_BOX_HEIGHT},
               .hitboxParts = {LOOKED_UP_BOXES.begin(), LOOKED_UP_BOXES.end()},
               .mask = nullptr,
               .hurtZone =
                   game::HurtZone{.boxes = {},
                                  .beams = {game::HurtBeam{
                                      .length = LOOKED_UP_BEAM_LENGTH,
                                      .thickness = LOOKED_UP_BEAM_THICKNESS}}}},
           game::NumberedEntityType{
               .name = std::string{TYPE_WITHOUT_APPEARANCE_NAME},
               .side = game::EntityType::Enemy,
               .hitboxParts = {},
               .mask = nullptr}},
      .arsenal = game::ShipArsenal{.gun = LOOKED_UP_SHIP_GUN, .weapons = {}}};
}

}  // namespace

EntityTypeLooksFixture::EntityTypeLooksFixture()
    : appearances_(lookedUpAppearance()), table_(twoEnemies()) {
  table_.drawLocalWith(appearances_);
}

const EntityTypeLooks& EntityTypeLooksFixture::looks() const { return looks_; }

const EntityAppearances& EntityTypeLooksFixture::appearances() const {
  return appearances_;
}

}  // namespace rtype::client::testing
