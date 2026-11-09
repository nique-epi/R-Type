#include "ReleaseManifestReadingFixtures.hpp"
#include <string>
#include <string_view>
#include "ReleaseManifestReadingTestConstants.hpp"

namespace rtype::client::testing {

std::string manifestWithPackage(std::string_view package) {
  return std::string{MANIFEST_START} + std::string{ONE_NOTE} +
         std::string{MANIFEST_PACKAGES} + std::string{package} +
         std::string{MANIFEST_END};
}

std::string manifestWithNotes(std::string_view notes) {
  return std::string{MANIFEST_START} + std::string{notes} +
         std::string{MANIFEST_PACKAGES} + std::string{GOOD_LINUX_PACKAGE} +
         std::string{MANIFEST_END};
}

}  // namespace rtype::client::testing
