#include <windows.h>  // NOLINT(misc-include-cleaner)
#include "ReducedMotion.hpp"

namespace rtype::client {

bool systemPrefersReducedMotion() {
  // NOLINTBEGIN(misc-include-cleaner)
  BOOL animationsShown = TRUE;
  if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animationsShown,
                            0) == FALSE) {
    return false;
  }
  return animationsShown == FALSE;
  // NOLINTEND(misc-include-cleaner)
}

}  // namespace rtype::client
