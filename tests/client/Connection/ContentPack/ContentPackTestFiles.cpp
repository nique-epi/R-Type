#include "ContentPackTestFiles.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFingerprint.hpp"
#include "ContentPack.hpp"
#include "ContentPackTestFilesConstants.hpp"

namespace rtype::client::testing {

std::vector<identifiers::ContentFile> lancerFiles() {
  return {identifiers::ContentFile{.path = std::string{LANCER_SHEET_PATH},
                                   .bytes = std::string{LANCER_SHEET}},
          identifiers::ContentFile{.path = std::string{LANCER_SPRITES_PATH},
                                   .bytes = std::string{LANCER_PIXELS}}};
}

std::vector<identifiers::ContentFile> lancerFilesNamingAFileOutside() {
  std::vector<identifiers::ContentFile> files = lancerFiles();
  files.front().bytes = std::string{SHEET_NAMING_A_FILE_OUTSIDE};
  return files;
}

std::vector<std::byte> lancerPack() {
  return network::encodeContentPack(lancerFiles());
}

std::vector<std::byte> packNumbered(std::size_t seed) {
  const std::vector<identifiers::ContentFile> files{identifiers::ContentFile{
      .path = std::string{LANCER_SPRITES_PATH}, .bytes = std::to_string(seed)}};
  return network::encodeContentPack(files);
}

network::ContentFingerprint fingerprintOf(const std::vector<std::byte>& pack) {
  return network::contentFingerprintOf(pack);
}

}  // namespace rtype::client::testing
