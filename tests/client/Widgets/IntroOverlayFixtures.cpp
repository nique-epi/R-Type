#include "IntroOverlayFixtures.hpp"
#include <SFML/Graphics/Rect.hpp>
#include "IntroOverlay.hpp"
#include "IntroOverlayTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextureArea.hpp"

InterfaceCanvasRecord overlayAt(float real) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintIntroOverlay(
      canvas, real,
      rtype::client::TextureArea{
          .assetId = rtype::client::testing::TITLE_LOGO_ID,
          .rectangle =
              sf::IntRect({0, 0}, rtype::client::testing::TITLE_LOGO_SIZE)});
  return record;
}
