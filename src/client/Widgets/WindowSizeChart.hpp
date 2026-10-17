#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

/**
 * @brief Paints every size of WINDOW_SIZES to scale, one inside the other
 * around one center, and its legend on the right: the size of the window, a
 * candidate size, and the sizes too large for the desktop, dashed.
 *
 * @param topLeft Top-left corner of the chart, WINDOW_SIZE_CHART_SIZE large,
 * in pixels of the interface picture.
 * @param candidate A size picked but not applied yet, such as the one
 * highlighted in an open list, by its index in WINDOW_SIZES; nothing, or the
 * size of the window, for none.
 */
void paintWindowSizeChart(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                          const WindowSizeSelection& selection,
                          std::optional<std::size_t> candidate);

}  // namespace rtype::client
