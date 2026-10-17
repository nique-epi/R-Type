#include <windows.h>
#include "ReducedMotion.hpp"

namespace rtype::client {

bool systemPrefersReducedMotion() {
  BOOL animationsShown = TRUE;
  if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animationsShown,
                            0) == FALSE) {
    return false;
  }
  return animationsShown == FALSE;
}

}  // namespace rtype::client
