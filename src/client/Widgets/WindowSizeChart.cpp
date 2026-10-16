#include "WindowSizeChart.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "Outline.hpp"
#include "PixelSize.hpp"
#include "TextStyle.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeChartConstants.hpp"
#include "WindowSizeSelection.hpp"
#include "WindowSizeText.hpp"

namespace rtype::client {

namespace {

/** @returns Where a size lies on the chart: to scale, centered on it. */
sf::IntRect chartAreaOf(PixelSize size, sf::Vector2i topLeft) {
  const sf::Vector2i scaled(
      static_cast<int>(size.width / WINDOW_SIZE_CHART_SCALE),
      static_cast<int>(size.height / WINDOW_SIZE_CHART_SCALE));
  const int sides = static_cast<int>(SIDES_SHARING_FREE_SPACE);
  return {topLeft + ((WINDOW_SIZE_CHART_SIZE - scaled) / sides), scaled};
}

/** @brief Writes a line of the legend, by its order from the top. */
void writeLegendLine(IInterfaceCanvas& canvas, sf::Vector2i topLeft, int line,
                     const std::string& text, sf::Color color) {
  canvas.write(text,
               topLeft + WINDOW_SIZE_LEGEND_OFFSET +
                   sf::Vector2i(0, line * WINDOW_SIZE_LEGEND_LINE_HEIGHT),
               {.font = TextFont::Small, .color = color});
}

}  // namespace

void paintWindowSizeChart(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                          const WindowSizeSelection& selection,
                          std::optional<std::size_t> candidate) {
  const std::size_t current = selection.selectedIndex();
  int line = 0;
  writeLegendLine(canvas, topLeft, line,
                  CURRENT_SIZE_TAG + windowSizeText(WINDOW_SIZES[current]),
                  CURRENT_SIZE_COLOR);
  ++line;
  for (std::size_t index = 0; index < WINDOW_SIZES.size(); ++index) {
    const sf::IntRect area = chartAreaOf(WINDOW_SIZES[index], topLeft);
    const std::string size = windowSizeText(WINDOW_SIZES[index]);
    if (!selection.isAvailable(index)) {
      Outline(area, Outline::Line::Dashed).paint(canvas, TOO_LARGE_SIZE_COLOR);
      writeLegendLine(canvas, topLeft, line, TOO_LARGE_SIZE_TAG + size,
                      TOO_LARGE_LEGEND_COLOR);
      ++line;
    } else if (index == current) {
      Outline(area, Outline::Line::Solid).paint(canvas, CURRENT_SIZE_COLOR);
    } else if (candidate == index) {
      Outline(area, Outline::Line::Solid).paint(canvas, CANDIDATE_SIZE_COLOR);
      writeLegendLine(canvas, topLeft, line, CANDIDATE_SIZE_TAG + size,
                      CANDIDATE_SIZE_COLOR);
      ++line;
    } else {
      Outline(area, Outline::Line::Solid).paint(canvas, SIZE_OUTLINE_COLOR);
    }
  }
}

}  // namespace rtype::client
