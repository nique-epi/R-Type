#pragma once

#include <cstdint>
#include <filesystem>
#include "ContentAnnouncement.hpp"

namespace rtype::server {

/**
 * @returns What a game announces of the content of @p assetFolder, served by
 * its supervisor on TCP @p port: the fingerprint and the size of the pack
 * ServerContentModule serves from the same folder.
 * @throws What game::contentFilesIn() and network::encodeContentPack() throw.
 */
[[nodiscard]] network::ContentAnnouncement contentAnnouncementOf(
    const std::filesystem::path& assetFolder, std::uint16_t port);

}  // namespace rtype::server
