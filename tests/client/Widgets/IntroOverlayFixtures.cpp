#include "IntroOverlayFixtures.hpp"
#include <SFML/Graphics/Rect.hpp>
#include "IntroOverlay.hpp"
#include "IntroOverlayLook.hpp"
#include "IntroOverlayTestConstants.hpp"
#include "PlayerPalette.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextureArea.hpp"

InterfaceCanvasRecord overlayAt(float real,
                                const rtype::client::PlayerPalette& palette) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintIntroOverlay(
      canvas, real,
      rtype::client::IntroOverlayLook{
          .logo =
              rtype::client::TextureArea{
                  .assetId = rtype::client::testing::TITLE_LOGO_ID,
                  .rectangle = sf::IntRect(
                      {0, 0}, rtype::client::testing::TITLE_LOGO_SIZE)},
          .palette = palette});
  return record;
}
