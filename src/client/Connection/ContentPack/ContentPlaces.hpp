#pragma once

#include <filesystem>

namespace rtype::client {

/** @brief Where a connection finds the client's own content, and where it
 * keeps the packs of the servers it meets. */
struct ContentPlaces {
  std::filesystem::path assetFolder;
  std::filesystem::path cacheFolder;
};

}  // namespace rtype::client
