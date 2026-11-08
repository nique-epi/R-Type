#include "ContentAnnouncementReading.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>
#include "ContentAnnouncement.hpp"
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentPack.hpp"

namespace rtype::server {

network::ContentAnnouncement contentAnnouncementOf(
    std::span<const std::filesystem::path> contentFolders, std::uint16_t port) {
  const std::vector<identifiers::ContentFile> files =
      game::contentFilesIn(contentFolders);
  const std::vector<std::byte> pack = network::encodeContentPack(files);
  return network::ContentAnnouncement{
      .fingerprint = network::contentFingerprintOf(pack),
      .size = static_cast<std::uint32_t>(pack.size()),
      .port = port};
}

}  // namespace rtype::server
