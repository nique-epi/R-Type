#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include "IInterfaceCanvas.hpp"
#include "Lagometer.hpp"
#include "LagometerLook.hpp"

namespace rtype::client {

/**
 * @brief Paints the panel of @p lagometer on @p canvas, at its place on the
 * interface picture, as large as @p look says: the fill at the opacity of
 * @p look, the border, the three lines of lagometerTextLines(), and the
 * labels of the scale on the right of the curve, the top and its half in
 * milliseconds.
 */
void paintLagometerPanel(IInterfaceCanvas& canvas, const Lagometer& lagometer,
                         LagometerLook look);

/**
 * @returns The curve of @p lagometer as lines, in coordinates of the
 * playfield, so it is drawn at the resolution of the window over the panel
 * paintLagometerPanel() painted at @p scale: the lines of the scale at 0, half
 * the top and the top, then a line from each point to the next, the newest on
 * the right edge, a round trip above the top drawn at the top, and a red line
 * across the curve at each point that missed a datagram.
 */
[[nodiscard]] sf::VertexArray lagometerCurve(const Lagometer& lagometer,
                                             int scale);

}  // namespace rtype::client
