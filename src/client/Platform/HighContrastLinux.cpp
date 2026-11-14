#include <string>
#include "CommandFirstLine.hpp"
#include "HighContrast.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

bool systemPrefersHighContrast() {
  return firstLineOf(PORTAL_CONTRAST_COMMAND).find(PORTAL_HIGHER_CONTRAST) !=
         std::string::npos;
}

}  // namespace rtype::client
