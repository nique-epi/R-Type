#include "EntityTypeLooksFixture.hpp"
#include <array>
#include <string>
#include <string_view>
#include "EntityAppearances.hpp"
#include "EntitySide.hpp"
#include "EntityTypeEntry.hpp"
#include "EntityTypeLooks.hpp"
#include "EntityTypeLooksTestConstants.hpp"
#include "SheetText.hpp"

namespace rtype::client::testing {

namespace {

/** @returns true: the table of these tests is never judged. */
bool drawsEveryType([[maybe_unused]] std::string_view name) { return true; }

/** @returns The appearance of LOOKED_UP_TYPE_NAME alone. */
EntityAppearances lookedUpAppearance() {
  const std::array sheets{
      game::SheetText{.name = std::string{LOOKED_UP_TYPE_NAME},
                      .source = std::string{LOOKED_UP_TYPE_NAME},
                      .text = std::string{LOOKED_UP_SHEET_TEXT}}};
  return EntityAppearances::fromTexts(sheets);
}

}  // namespace

EntityTypeLooksFixture::EntityTypeLooksFixture()
    : table_(drawsEveryType), appearances_(lookedUpAppearance()) {
  table_.keep(
      {network::EntityTypeEntry{.side = network::EntitySide::Enemy,
                                .name = std::string{LOOKED_UP_TYPE_NAME}},
       network::EntityTypeEntry{
           .side = network::EntitySide::Enemy,
           .name = std::string{TYPE_WITHOUT_APPEARANCE_NAME}}});
}

const EntityTypeLooks& EntityTypeLooksFixture::looks() const { return looks_; }

const EntityAppearances& EntityTypeLooksFixture::appearances() const {
  return appearances_;
}

}  // namespace rtype::client::testing
