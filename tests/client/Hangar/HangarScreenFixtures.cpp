#include "HangarScreenFixtures.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "HangarScreenTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

namespace rtype::client::testing {

std::size_t lookImagesAtShipScale(const InterfaceCanvasRecord& painting,
                                  std::optional<sf::IntRect> source) {
  return static_cast<std::size_t>(std::ranges::count_if(
      painting.images, [source](const RecordedImage& image) {
        return image.assetId == LOOK_SHEET &&
               image.scale == SHIP_SCALE_OF_THE_HANGAR &&
               (!source.has_value() || image.source == *source);
      }));
}

}  // namespace rtype::client::testing
