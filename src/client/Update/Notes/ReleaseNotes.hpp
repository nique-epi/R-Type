#pragma once

#include <string>
#include <vector>

namespace rtype::client {

/** @brief The notes of a release, kept for WHAT'S NEW: the version they
 * describe, as written, and their lines. */
struct ReleaseNotes {
  std::string versionText;
  std::vector<std::string> lines;
};

}  // namespace rtype::client
