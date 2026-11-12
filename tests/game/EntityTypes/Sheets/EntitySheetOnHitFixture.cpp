#include "EntitySheetOnHitFixture.hpp"
#include <exception>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include "BuiltInBehaviours.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetOnHitTestConstants.hpp"
#include "EntitySheetReader.hpp"
#include "EntitySheetReaderTestConstants.hpp"
#include "IHitHandler.hpp"
#include "RecordingHitHandler.hpp"
#include "SheetText.hpp"

namespace rtype::game::testing {

EntitySheetOnHitFixture::EntitySheetOnHitFixture()
    : behaviours_(builtInBehaviours()) {
  behaviours_.addHitHandler(
      std::make_unique<RecordingHitHandler>(ON_HIT_HANDLER_NAME));
  handler_ = behaviours_.findHitHandler(ON_HIT_HANDLER_NAME);
}

EntitySheet EntitySheetOnHitFixture::read(
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

std::string EntitySheetOnHitFixture::refusalOf(
    std::initializer_list<std::string_view> parts) const {
  try {
    static_cast<void>(read(parts));
  } catch (const std::exception& error) {
    return error.what();
  }
  return {};
}

IHitHandler* EntitySheetOnHitFixture::handler() const { return handler_; }

}  // namespace rtype::game::testing
