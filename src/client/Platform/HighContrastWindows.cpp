#include <windows.h>
#include "HighContrast.hpp"

namespace rtype::client {

bool systemPrefersHighContrast() {
  HIGHCONTRASTW contrast{};
  contrast.cbSize = sizeof(contrast);
  if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast,
                            0) == FALSE) {
    return false;
  }
  return (contrast.dwFlags & DWORD{HCF_HIGHCONTRASTON}) != 0;
}

}  // namespace rtype::client
