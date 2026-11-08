#include "ContentAnnouncementReading.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>
#include "ContentAnnouncement.hpp"
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentPack.hpp"

namespace rtype::server {

network::ContentAnnouncement contentAnnouncementOf(
    const std::filesystem::path& assetFolder, std::uint16_t port) {
  const std::vector<identifiers::ContentFile> files =
      game::contentFilesIn(assetFolder);
  const std::vector<std::byte> pack = network::encodeContentPack(files);
  return network::ContentAnnouncement{
      .fingerprint = network::contentFingerprintOf(pack),
      .size = static_cast<std::uint32_t>(pack.size()),
      .port = port};
}

}  // namespace rtype::server
