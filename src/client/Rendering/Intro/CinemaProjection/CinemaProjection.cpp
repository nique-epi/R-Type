#include "CinemaProjection.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "IntroFrame.hpp"
#include "PixelRounding.hpp"
#include "WorldArea.hpp"
#include "WorldImage.hpp"

namespace rtype::client {

namespace {

/** @returns @p topLeft, shifted left by the drift of the frame when it is
 * debris. */
sf::Vector2f placedInWorld(const IntroFrame& frame, sf::Vector2f topLeft,
                           bool leftBehind) {
  return leftBehind ? topLeft - sf::Vector2f(frame.drift, 0.0F) : topLeft;
}

}  // namespace

sf::Vector2i canvasPointOf(const IntroFrame& frame, sf::Vector2f world) {
  return ((nearestPixel(world) - frame.view.topLeft) * frame.view.zoom) +
         frame.shake;
}

void paintWorldArea(IInterfaceCanvas& canvas, const IntroFrame& frame,
                    const WorldArea& area) {
  canvas.fill(
      sf::IntRect(canvasPointOf(frame, placedInWorld(frame, area.topLeft,
                                                     area.leftBehind)),
                  area.size * frame.view.zoom),
      area.color);
}

void drawWorldImage(IInterfaceCanvas& canvas, const IntroFrame& frame,
                    const WorldImage& image) {
  canvas.drawImage(image.image,
                   ImagePlacement{.topLeft = canvasPointOf(
                                      frame, placedInWorld(frame, image.topLeft,
                                                           image.leftBehind)),
                                  .scale = frame.view.zoom,
                                  .tint = image.tint,
                                  .recolor = image.recolor},
                   INTERFACE_AREA);
}

}  // namespace rtype::client
