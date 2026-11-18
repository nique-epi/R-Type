#pragma once

#include <filesystem>

namespace rtype::client {

/** @brief Where the content of a pack is read: the assets of the client,
 * where a background of the pack finds a file the pack does not bring, and
 * the folder the files of the pack are written below. */
struct PackFolders {
  std::filesystem::path assets;
  std::filesystem::path pack;
};

}  // namespace rtype::client
