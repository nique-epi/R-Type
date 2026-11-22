#include "HeadphonesScreen.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "ColoredPixel.hpp"
#include "Easing.hpp"
#include "HeadphonesScreenConstants.hpp"
#include "HeadphonesScreenParts.hpp"
#include "HeldKey.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "IntroOverlay.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The color a letter of HEADPHONES_LOGO paints, or nothing for a
 * clear pixel. */
std::optional<sf::Color> logoColorOf(char letter) {
  switch (letter) {
    case HEADPHONES_INK_LETTER:
      return INK;
    case HEADPHONES_CYAN_LETTER:
      return HUD_CYAN;
    case HEADPHONES_EDGE_BLUE_LETTER:
      return EDGE_BLUE;
    case HEADPHONES_DEEP_BLUE_LETTER:
      return DEEP_BLUE;
    case HEADPHONES_BLUE_LETTER:
      return BLUE;
    case HEADPHONES_PALE_BLUE_LETTER:
      return PALE_BLUE;
    default:
      return std::nullopt;
  }
}

/** @returns Every pixel the logo paints, its top-left corner at
 * HEADPHONES_LOGO_TOP_LEFT. */
std::vector<ColoredPixel> logoPixels() {
  std::vector<ColoredPixel> pixels;
  for (std::size_t row = 0; row < HEADPHONES_LOGO.size(); ++row) {
    const std::string_view line = HEADPHONES_LOGO[row];
    for (std::size_t column = 0; column < line.size(); ++column) {
      if (const std::optional<sf::Color> color = logoColorOf(line[column])) {
        const sf::Vector2i offset{static_cast<int>(column),
                                  static_cast<int>(row)};
        pixels.push_back(
            {.position = HEADPHONES_LOGO_TOP_LEFT + offset, .color = *color});
      }
    }
  }
  return pixels;
}

}  // namespace

HeadphonesScreen::HeadphonesScreen(HeadphonesScreenParts parts,
                                   std::unique_ptr<IInterfaceCanvas> canvas)
    : canvas_(std::move(canvas)),
      navigator_(&parts.navigator.get()),
      next_(parts.next),
      logo_(logoPixels()) {
  repaint();
}

void HeadphonesScreen::handleEvent(const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    heldKey_ = pressed->code;
  } else if (releasesHeldKey(event, heldKey_)) {
    heldKey_.reset();
    if (event.is<sf::Event::KeyReleased>()) {
      leave();
    }
  } else if (event.is<sf::Event::MouseButtonPressed>()) {
    leave();
  }
}

void HeadphonesScreen::update(engine::Duration elapsed) {
  since_ += elapsed;
  if (since_ <
      leavingFrom_ + HEADPHONES_FADE_OUT + HEADPHONES_INK_BEFORE_NEXT) {
    if (shareOfLight() != paintedShare_) {
      repaint();
    }
    return;
  }
  if (heldKey_.has_value() || std::exchange(nextAsked_, true)) {
    return;
  }
  navigator_->replaceAll(next_);
}

void HeadphonesScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool HeadphonesScreen::hidesScreensBelow() const { return true; }

void HeadphonesScreen::leave() {
  const auto fadeAlreadyBehind = std::chrono::duration_cast<engine::Duration>(
      HEADPHONES_FADE_OUT * (1.0F - shareOfLight()));
  leavingFrom_ = std::min(leavingFrom_, since_ - fadeAlreadyBehind);
}

float HeadphonesScreen::shareOfLight() const {
  return std::min(
      progressBetween(since_, {}, HEADPHONES_FADE_IN),
      1.0F - progressBetween(since_, leavingFrom_, HEADPHONES_FADE_OUT));
}

void HeadphonesScreen::repaint() {
  paintedShare_ = shareOfLight();
  canvas_->clear();
  canvas_->fill(INTERFACE_AREA, INK);
  canvas_->plot(logo_);
  canvas_->write(HEADPHONES_LINE, HEADPHONES_LINE_ANCHOR,
                 HEADPHONES_LINE_STYLE);
  canvas_->write(HEADPHONES_SMALL_LINE, HEADPHONES_SMALL_LINE_ANCHOR,
                 HEADPHONES_SMALL_LINE_STYLE);
  paintInkCover(*canvas_, 1.0F - paintedShare_);
  canvas_->display();
}

}  // namespace rtype::client
