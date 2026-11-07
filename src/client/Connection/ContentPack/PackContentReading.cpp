#include "PackContentReading.hpp"
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <vector>
#include "ClientException.hpp"
#include "ContentFile.hpp"
#include "DataFileConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntitySheetConstants.hpp"
#include "NumberedEntityTypes.hpp"
#include "PackContent.hpp"
#include "SheetText.hpp"

namespace rtype::client {

namespace {

/** @returns The type @p file is the sheet of, or nothing when it is no sheet:
 * a `.toml` file right in ENTITY_SHEET_FOLDER_IN_ASSETS. */
std::optional<std::string> sheetNameOf(const identifiers::ContentFile& file) {
  const std::filesystem::path path{file.path};
  if (path.parent_path() != game::ENTITY_SHEET_FOLDER_IN_ASSETS ||
      path.extension() != engine::DATA_FILE_EXTENSION) {
    return std::nullopt;
  }
  return path.stem().string();
}

/** @brief Writes @p file below @p folder, with the folders on its way.
 * @throws ContentFileWriteException when the disk refuses it. */
void write(const identifiers::ContentFile& file,
           const std::filesystem::path& folder) {
  const std::filesystem::path written = folder / file.path;
  std::error_code ignored;
  std::filesystem::create_directories(written.parent_path(), ignored);
  std::ofstream stream{written, std::ios::binary | std::ios::trunc};
  stream.write(file.bytes.data(),
               static_cast<std::streamsize>(file.bytes.size()));
  if (!stream) {
    throw ContentFileWriteException(written.generic_string());
  }
}

}  // namespace

PackContent packContentOf(std::span<const identifiers::ContentFile> files,
                          const std::filesystem::path& spriteFolder) {
  std::vector<game::SheetText> sheets;
  for (const identifiers::ContentFile& file : files) {
    const std::optional<std::string> name = sheetNameOf(file);
    if (name.has_value()) {
      sheets.push_back(
          game::SheetText{.name = *name,
                          .source = (spriteFolder / file.path).generic_string(),
                          .text = file.bytes});
    } else {
      write(file, spriteFolder);
    }
  }
  return PackContent{
      .appearances = EntityAppearances::fromTexts(sheets, spriteFolder),
      .types = game::numberedEntityTypesOf(sheets)};
}

}  // namespace rtype::client
