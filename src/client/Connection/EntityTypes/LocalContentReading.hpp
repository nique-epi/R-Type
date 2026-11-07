#pragma once

#include <filesystem>
#include "LocalContent.hpp"

namespace rtype::client {

/**
 * @returns The content of @p assetFolder, as a server whose assets are the
 * same announces it: the fingerprint of the pack game::contentFilesIn() lists
 * there, and the types of its sheets.
 * @throws What game::contentFilesIn(), network::encodeContentPack() and
 * game::numberedEntityTypesOf() throw.
 */
[[nodiscard]] LocalContent localContentOf(
    const std::filesystem::path& assetFolder);

}  // namespace rtype::client
