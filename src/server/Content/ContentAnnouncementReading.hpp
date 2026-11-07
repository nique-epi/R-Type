#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include "ContentAnnouncement.hpp"

namespace rtype::server {

/**
 * @returns What a game announces of the content of @p contentFolders, served
 * by its supervisor on TCP @p port: the fingerprint and the size of the pack
 * ServerContentModule serves from the same folders.
 * @throws What game::contentFilesIn() and network::encodeContentPack() throw.
 */
[[nodiscard]] network::ContentAnnouncement contentAnnouncementOf(
    std::span<const std::filesystem::path> contentFolders, std::uint16_t port);

}  // namespace rtype::server
