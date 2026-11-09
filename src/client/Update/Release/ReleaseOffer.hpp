#pragma once

#include <string>
#include <vector>
#include "ReleasePackage.hpp"
#include "ReleaseVersion.hpp"

namespace rtype::client {

/** @brief What a signed manifest offers this system: the version, as
 * numbers and as written, the notes of WHAT'S NEW, and the package. */
struct ReleaseOffer {
  ReleaseVersion version;
  std::string versionText;
  std::vector<std::string> notes;
  ReleasePackage package;
};

}  // namespace rtype::client
