#pragma once

#include <SFML/System/Vector2.hpp>
#include "IInterfaceCanvas.hpp"
#include "IntroFrame.hpp"
#include "WorldArea.hpp"
#include "WorldImage.hpp"

namespace rtype::client {

/** @returns The pixel of the canvas a point of the world lands on in that
 * frame: rounded to a pixel of the world, less the top-left corner of the
 * view, times the zoom, plus the shake. */
[[nodiscard]] sf::Vector2i canvasPointOf(const IntroFrame& frame,
                                         sf::Vector2f world);

/** @brief Paints a rectangle of the world, each pixel of the world zoom ×
 * zoom pixels of the canvas, shifted left by the drift when it is debris. */
void paintWorldArea(IInterfaceCanvas& canvas, const IntroFrame& frame,
                    const WorldArea& area);

/** @brief Lays an image of a sheet on the world, enlarged by the zoom,
 * shifted left by the drift when it is debris. */
void drawWorldImage(IInterfaceCanvas& canvas, const IntroFrame& frame,
                    const WorldImage& image);

}  // namespace rtype::client
