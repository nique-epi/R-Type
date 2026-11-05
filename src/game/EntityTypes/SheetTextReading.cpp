#include "SheetTextReading.hpp"
#include <filesystem>
#include <span>
#include <string_view>
#include <vector>
#include "DataFileReading.hpp"
#include "EntitySheetConstants.hpp"
#include "GameException.hpp"
#include "PathText.hpp"
#include "SheetText.hpp"

namespace rtype::game {

namespace {

/** @returns Whether @p name may name a type: 1 to
 * MAXIMUM_ENTITY_TYPE_NAME_LENGTH bytes of ENTITY_TYPE_NAME_CHARACTERS. */
bool isEntityTypeName(std::string_view name) {
  return !name.empty() && name.size() <= MAXIMUM_ENTITY_TYPE_NAME_LENGTH &&
         name.find_first_not_of(ENTITY_TYPE_NAME_CHARACTERS) ==
             std::string_view::npos;
}

}  // namespace

std::vector<SheetText> sheetTextsIn(const std::filesystem::path& folder) {
  const std::vector<std::filesystem::path> files = engine::dataFilesIn(folder);
  if (files.empty()) {
    throw NoEntitySheetException(engine::shownPath(folder));
  }
  std::vector<SheetText> texts;
  texts.reserve(files.size());
  for (const std::filesystem::path& file : files) {
    texts.push_back(SheetText{.name = engine::genericText(file.stem()),
                              .source = engine::shownPath(file),
                              .text = engine::dataFileContent(file)});
  }
  checkSheetNames(texts);
  return texts;
}

void checkSheetNames(std::span<const SheetText> texts) {
  if (texts.size() > MAXIMUM_ENTITY_TYPES) {
    throw TooManyEntityTypesException(texts.size(), MAXIMUM_ENTITY_TYPES);
  }
  for (const SheetText& text : texts) {
    if (!isEntityTypeName(text.name)) {
      throw InvalidEntityTypeNameException(text.source,
                                           MAXIMUM_ENTITY_TYPE_NAME_LENGTH);
    }
  }
}

}  // namespace rtype::game
