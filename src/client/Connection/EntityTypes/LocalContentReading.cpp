#include "LocalContentReading.hpp"
#include <cstddef>
#include <filesystem>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentPack.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityTypes.hpp"

namespace rtype::client {

LocalContent localContentOf(const std::filesystem::path& assetFolder) {
  const std::vector<identifiers::ContentFile> files =
      game::contentFilesIn(assetFolder);
  const std::vector<std::byte> pack = network::encodeContentPack(files);
  return LocalContent{.fingerprint = network::contentFingerprintOf(pack),
                      .types = game::numberedEntityTypesOf(
                          game::sheetTextsOf(files, assetFolder))};
}

}  // namespace rtype::client
