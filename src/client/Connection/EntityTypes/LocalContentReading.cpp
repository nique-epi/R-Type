#include "LocalContentReading.hpp"
#include <cstddef>
#include <filesystem>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentPack.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityTypes.hpp"
#include "SheetText.hpp"
#include "ShipGunReading.hpp"

namespace rtype::client {

LocalContent localContentOf(const std::filesystem::path& assetFolder) {
  const std::vector<identifiers::ContentFile> files =
      game::contentFilesIn(assetFolder);
  const std::vector<std::byte> pack = network::encodeContentPack(files);
  const std::vector<game::SheetText> sheets =
      game::sheetTextsOf(files, assetFolder);
  return LocalContent{.fingerprint = network::contentFingerprintOf(pack),
                      .types = game::numberedEntityTypesOf(sheets),
                      .gun = game::shipGunOrNoneOf(sheets)};
}

}  // namespace rtype::client
