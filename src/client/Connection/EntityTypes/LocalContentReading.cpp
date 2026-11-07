#include "LocalContentReading.hpp"
#include <cstddef>
#include <filesystem>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentPack.hpp"
#include "EntitySheetConstants.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityTypes.hpp"
#include "SheetTextReading.hpp"

namespace rtype::client {

LocalContent localContentOf(const std::filesystem::path& assetFolder) {
  const std::vector<identifiers::ContentFile> files =
      game::contentFilesIn(assetFolder);
  const std::vector<std::byte> pack = network::encodeContentPack(files);
  return LocalContent{.fingerprint = network::contentFingerprintOf(pack),
                      .types = game::numberedEntityTypesOf(game::sheetTextsIn(
                          assetFolder / game::ENTITY_SHEET_FOLDER_IN_ASSETS))};
}

}  // namespace rtype::client
