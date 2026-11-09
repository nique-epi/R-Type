#pragma once

#include <cstddef>
#include <string>

namespace rtype::client {

/** @brief The package of one system in a manifest: its file name in the
 * release, its size in bytes, and its SHA-256 in lowercase hexadecimal. */
struct ReleasePackage {
  std::string file;
  std::size_t size{};
  std::string sha256;
};

}  // namespace rtype::client
