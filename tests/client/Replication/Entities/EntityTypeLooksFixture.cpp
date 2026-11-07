#include "EntityTypeLooksFixture.hpp"
#include <array>
#include <string>
#include "EntityAppearances.hpp"
#include "EntityType.hpp"
#include "EntityTypeLooks.hpp"
#include "EntityTypeLooksTestConstants.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityType.hpp"
#include "SheetTextFixtures.hpp"

namespace rtype::client::testing {

namespace {

/** @returns The appearance of LOOKED_UP_TYPE_NAME alone. */
EntityAppearances lookedUpAppearance() {
  const std::array sheets{
      game::testing::sheetText(LOOKED_UP_TYPE_NAME, LOOKED_UP_SHEET_TEXT)};
  return EntityAppearances::fromTexts(sheets);
}

/** @returns The client's own content of these tests: LOOKED_UP_TYPE_NAME,
 * then TYPE_WITHOUT_APPEARANCE_NAME, both enemies. */
LocalContent twoEnemies() {
  return LocalContent{
      .fingerprint = {},
      .types = {
          game::NumberedEntityType{.name = std::string{LOOKED_UP_TYPE_NAME},
                                   .side = game::EntityType::Enemy},
          game::NumberedEntityType{
              .name = std::string{TYPE_WITHOUT_APPEARANCE_NAME},
              .side = game::EntityType::Enemy}}};
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
