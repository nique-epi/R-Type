#include "EntitySheetReaderFixture.hpp"
#include <exception>
#include <initializer_list>
#include <string>
#include <string_view>
#include "BuiltInBehaviours.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetReader.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "SheetText.hpp"

namespace rtype::game::testing {

EntitySheetReaderFixture::EntitySheetReaderFixture()
    : behaviours_(builtInBehaviours()) {}

EntitySheet EntitySheetReaderFixture::read(
    std::initializer_list<std::string_view> parts) const {
  std::string text;
  for (const std::string_view part : parts) {
    text += part;
  }
  return gameplayOf(SheetText{.name = std::string{READ_SHEET_NAME},
                              .source = std::string{READ_SHEET_SOURCE},
                              .text = text},
                    behaviours_);
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
