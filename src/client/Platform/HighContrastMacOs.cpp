#include "HighContrast.hpp"
#include "MacOsWorkspaceFlag.hpp"
#include "PlatformConstants.hpp"

namespace rtype::client {

bool systemPrefersHighContrast() {
  return macOsWorkspaceFlag(MACOS_INCREASE_CONTRAST_SELECTOR);
}

}  // namespace rtype::client
