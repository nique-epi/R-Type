#include "BrowserOpening.hpp"
#include <string>
#include "PlatformConstants.hpp"

namespace rtype::client {

bool isWebAddress(const std::string& url) {
  return url.starts_with(HTTPS_URL_START) || url.starts_with(HTTP_URL_START);
}

}  // namespace rtype::client
