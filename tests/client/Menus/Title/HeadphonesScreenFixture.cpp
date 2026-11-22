#include "HeadphonesScreenFixture.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <limits>
#include <memory>
#include "HeadphonesScreen.hpp"
#include "HeadphonesScreenParts.hpp"
#include "HeadphonesScreenTestConstants.hpp"
#include "MenuScreenIdConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::HeadphonesScreen;
using rtype::client::HeadphonesScreenParts;
using rtype::client::INTRO_SCREEN;
using rtype::client::testing::ONE_PLOTTED_PIXEL;

void HeadphonesScreenFixture::SetUp() {
  screen_ = std::make_unique<HeadphonesScreen>(
      HeadphonesScreenParts{.navigator = navigator_, .next = INTRO_SCREEN},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

HeadphonesScreen& HeadphonesScreenFixture::screen() { return *screen_; }

const RecordingNavigator& HeadphonesScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& HeadphonesScreenFixture::record() const {
  return canvas_;
}

NavigatorCall HeadphonesScreenFixture::introAsked() {
  return NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                       .screen = INTRO_SCREEN};
}

sf::IntRect HeadphonesScreenFixture::plottedBounds() const {
  sf::Vector2i lowest{std::numeric_limits<int>::max(),
                      std::numeric_limits<int>::max()};
  sf::Vector2i highest{std::numeric_limits<int>::min(),
                       std::numeric_limits<int>::min()};
  for (const RecordedFill& fill : canvas_.fills) {
    if (fill.area.size != ONE_PLOTTED_PIXEL) {
      continue;
    }
    lowest = {std::min(lowest.x, fill.area.position.x),
              std::min(lowest.y, fill.area.position.y)};
    highest = {std::max(highest.x, fill.area.position.x),
               std::max(highest.y, fill.area.position.y)};
  }
  return {lowest, highest - lowest + ONE_PLOTTED_PIXEL};
}
