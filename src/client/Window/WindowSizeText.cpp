#include "WindowSizeText.hpp"
#include <string>
#include "PixelSize.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

std::string windowSizeText(PixelSize size) {
  std::string text = std::to_string(size.width);
  text += WINDOW_SIZE_TEXT_SEPARATOR;
  text += std::to_string(size.height);
  return text;
}

}  // namespace rtype::client
