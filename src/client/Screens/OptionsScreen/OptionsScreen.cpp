#include "OptionsScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include "Button.hpp"
#include "GameWindow.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PixelSize.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "TimeConstants.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

namespace {

/** @returns The label of the bar of a window size, such as "1920 X 1080". */
std::string windowSizeLabel(PixelSize size) {
  return std::to_string(size.width) + WINDOW_SIZE_SEPARATOR +
         std::to_string(size.height);
}

}  // namespace

OptionsScreen::OptionsScreen(IScreenNavigator& navigator, GameWindow& window,
                             std::unique_ptr<IInterfaceCanvas> canvas,
                             PlayfieldMapping playfieldPointAt)
    : window_(&window),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(playfieldPointAt)),
      menu_(buildMenu(navigator, window)) {
  showWindowSizes();
  repaint();
}

void OptionsScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, playfieldPointAt_);
}

void OptionsScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  showWindowSizes();
  repaint();
}

void OptionsScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool OptionsScreen::hidesScreensBelow() const { return true; }

Menu OptionsScreen::buildMenu(IScreenNavigator& navigator, GameWindow& window) {
  MenuBuilder builder(MenuBuilder::Frame::Page);
  builder.heading(SETTINGS_HEADING);
  for (std::size_t sizeIndex = 0; sizeIndex < WINDOW_SIZES.size();
       ++sizeIndex) {
    builder.button(
        {.label = windowSizeLabel(WINDOW_SIZES[sizeIndex])},
        [&window, sizeIndex] { window.selectWindowSize(sizeIndex); });
  }
  builder.backButton(navigator);
  return builder.build();
}

void OptionsScreen::showWindowSizes() {
  const WindowSizeSelection& selection = window_->windowSizeSelection();
  for (std::size_t sizeIndex = 0; sizeIndex < WINDOW_SIZES.size();
       ++sizeIndex) {
    menu_.setEnabled(sizeIndex, selection.isAvailable(sizeIndex));
    menu_.setMark(sizeIndex, sizeIndex == selection.selectedIndex()
                                 ? Button::CheckMark::Checked
                                 : Button::CheckMark::Unchecked);
  }
}

void OptionsScreen::repaint() {
  canvas_->clear();
  menu_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
