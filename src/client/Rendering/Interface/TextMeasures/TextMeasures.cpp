#include "TextMeasures.hpp"
#include <SFML/System/Utf.hpp>
#include <algorithm>
#include <cstddef>
#include <string_view>
#include "InterfaceConstants.hpp"
#include "TextMetrics.hpp"
#include "TextSize.hpp"

namespace rtype::client {

int lineHeightOf(TextMetrics metrics) {
  return widthAtSizeOf(metrics, TEXT_LINE_HEIGHT);
}

int regularWidthOf(TextMetrics metrics, std::string_view text) {
  return static_cast<int>(sf::Utf8::count(text.begin(), text.end())) *
         lineHeightOf(metrics);
}

int widthAtSizeOf(TextMetrics metrics, int nativeWidth) {
  const int halves = nativeWidth * metrics.halfPixelsPerFontPixel;
  return (halves + HALF_PIXELS_PER_PIXEL - 1) / HALF_PIXELS_PER_PIXEL;
}

TextMetrics textMetricsOf(TextSize size) {
  return TextMetrics{.halfPixelsPerFontPixel =
                         TEXT_SIZE_HALF_PIXELS[static_cast<std::size_t>(size)]};
}

int halfPixelsOf(TextMetrics metrics, int styleScale) {
  return std::max(styleScale * HALF_PIXELS_PER_PIXEL,
                  metrics.halfPixelsPerFontPixel);
}

}  // namespace rtype::client
