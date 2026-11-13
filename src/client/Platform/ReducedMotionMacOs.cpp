#include "MacOsWorkspaceFlag.hpp"
#include "PlatformConstants.hpp"
#include "ReducedMotion.hpp"

namespace rtype::client {

bool systemPrefersReducedMotion() {
  return macOsWorkspaceFlag(MACOS_REDUCE_MOTION_SELECTOR);
}

}  // namespace rtype::client
