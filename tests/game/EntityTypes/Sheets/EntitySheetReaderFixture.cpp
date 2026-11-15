#include "EntitySheetReaderFixture.hpp"
#include <exception>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "BuiltInBehaviours.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetReader.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "SheetMaskText.hpp"
#include "SheetText.hpp"

namespace rtype::game::testing {

EntitySheetReaderFixture::EntitySheetReaderFixture()
    : behaviours_(builtInBehaviours()) {}

namespace {

/** @returns The sheet READ_SHEET_NAME whose text is @p parts, one after the
 * other, with @p mask as its mask file. */
SheetText sheetOf(std::initializer_list<std::string_view> parts,
                  std::optional<SheetMaskText> mask) {
  std::string text;
  for (const std::string_view part : parts) {
    text += part;
  }
  return SheetText{.name = std::string{READ_SHEET_NAME},
                   .source = std::string{READ_SHEET_SOURCE},
                   .text = text,
                   .mask = std::move(mask)};
}

}  // namespace

EntitySheet EntitySheetReaderFixture::read(
    std::initializer_list<std::string_view> parts) const {
  return gameplayOf(sheetOf(parts, std::nullopt), behaviours_);
}

EntitySheet EntitySheetReaderFixture::readWithMask(
    std::initializer_list<std::string_view> parts,
    std::string_view mask) const {
  return gameplayOf(
      sheetOf(parts, SheetMaskText{.source = std::string{READ_MASK_SOURCE},
                                   .text = std::string{mask}}),
      behaviours_);
}

std::string EntitySheetReaderFixture::maskRefusalOf(
    std::initializer_list<std::string_view> parts,
    std::string_view mask) const {
  try {
    static_cast<void>(readWithMask(parts, mask));
  } catch (const std::exception& error) {
    return error.what();
  }
  return {};
}

std::string EntitySheetReaderFixture::refusalOf(
    std::initializer_list<std::string_view> parts) const {
  try {
    static_cast<void>(read(parts));
  } catch (const std::exception& error) {
    return error.what();
  }
  return {};
}

}  // namespace rtype::game::testing
