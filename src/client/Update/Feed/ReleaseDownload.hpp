#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

namespace rtype::client {

/** @brief A file of a release to save: where it is under RELEASES_ORIGIN, the
 * file it is written to, its size as the signed feed gives it, and what
 * receives the share received, from 0 to FULL_DOWNLOAD_PERCENT. */
struct ReleaseDownload {
  std::string path;
  std::filesystem::path file;
  std::uint64_t size{};
  std::function<void(int)> progress;
};

}  // namespace rtype::client
