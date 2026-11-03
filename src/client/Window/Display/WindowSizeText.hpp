#pragma once

#include <string>
#include "PixelSize.hpp"

namespace rtype::client {

/** @returns A window size as the options write it: its width,
 * WINDOW_SIZE_TEXT_SEPARATOR, then its height, such as "1920 X 1080". */
[[nodiscard]] std::string windowSizeText(PixelSize size);

}  // namespace rtype::client
