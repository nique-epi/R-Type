#include "CommandFirstLine.hpp"
#include "PlatformConstants.hpp"
#include "ReducedMotion.hpp"

namespace rtype::client {

bool systemPrefersReducedMotion() {
  return firstLineOf(GNOME_ANIMATIONS_COMMAND)
      .starts_with(GNOME_ANIMATIONS_OFF);
}

}  // namespace rtype::client
