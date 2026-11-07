#include "ContentPackRoute.hpp"
#include <string>
#include "ContentFingerprint.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentRouteConstants.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"

namespace rtype::network {

std::string contentPackPathOf(const ContentFingerprint& fingerprint) {
  std::string path = apiPathOf(CONTENT_PACK_ROUTE);
  const std::string parameter = std::string{PATH_PARAMETER_PREFIX} +
                                std::string{FINGERPRINT_PATH_PARAMETER};
  path.replace(path.find(parameter), parameter.size(),
               fingerprintText(fingerprint));
  return path;
}

}  // namespace rtype::network
