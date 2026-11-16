#include "SeamCover.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "BackgroundConstants.hpp"
#include "BayerThreshold.hpp"
#include "BiomeConstants.hpp"
#include "DitherConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The pixels of the background @p canvas shows through its
 * view. */
sf::FloatRect shownArea(const sf::RenderTarget& canvas) {
  const sf::View& view = canvas.getView();
  return {view.getCenter() - (view.getSize() * VIEW_HALF_SHARE),
          view.getSize()};
}

}  // namespace

bool hiddenBySeam(sf::Vector2i fromSeam) {
  if (fromSeam.x < 0) {
    return true;
  }
  if (fromSeam.x >= SEAM_EDGE_WIDTH) {
    return false;
  }
  const double shown = (static_cast<double>(fromSeam.x) + BAYER_CELL_CENTRE) /
                       static_cast<double>(SEAM_EDGE_WIDTH);
  return bayerThresholdAt(fromSeam) >= shown;
}

bool hidesTheWholeCanvas(const sf::RenderTarget& canvas, int seam) {
  const sf::FloatRect shown = shownArea(canvas);
  return static_cast<float>(seam) >= shown.position.x + shown.size.x;
}

SeamCover::SeamCover() {
  const int rows = static_cast<int>(BACKGROUND_HEIGHT) + BAYER_SIZE;
  for (int column = 0; column < SEAM_EDGE_WIDTH; ++column) {
    for (int row = 0; row < rows; ++row) {
      if (hiddenBySeam({column, row})) {
        edge_.plot({column, row}, sf::Color::Transparent);
      }
    }
  }
}

void SeamCover::cover(sf::RenderTarget& canvas, int seam) const {
  const sf::FloatRect shown = shownArea(canvas);
  const float left = std::floor(shown.position.x);
  const float top = std::floor(shown.position.y / BAYER_SIZE) * BAYER_SIZE;
  if (static_cast<float>(seam) > left) {
    sf::RectangleShape cover({static_cast<float>(seam) - left, shown.size.y});
    cover.setPosition({left, shown.position.y});
    cover.setFillColor(sf::Color::Transparent);
    canvas.draw(cover, sf::BlendNone);
  }
  sf::RenderStates states(sf::BlendNone);
  states.transform.translate({static_cast<float>(seam), top});
  canvas.draw(edge_, states);
}

}  // namespace rtype::client
