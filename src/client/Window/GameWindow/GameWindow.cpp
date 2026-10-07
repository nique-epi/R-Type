#include "GameWindow.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <cstddef>
#include <optional>
#include "PixelPosition.hpp"
#include "PixelSize.hpp"
#include "PlayfieldConstants.hpp"
#include "PlayfieldViewport.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

PixelSize GameWindow::desktopSize() {
  const sf::Vector2u size = sf::VideoMode::getDesktopMode().size;
  return {.width = size.x, .height = size.y};
}

GameWindow::GameWindow()
    : windowSizeSelection_(desktopSize()),
      window_(sf::VideoMode({windowSizeSelection_.selected().width,
                             windowSizeSelection_.selected().height}),
              WINDOW_TITLE, sf::Style::Titlebar | sf::Style::Close) {
  window_.setFramerateLimit(FRAMES_PER_SECOND_LIMIT);
  applySelectedWindowSize();
}

bool GameWindow::isOpen() const { return window_.isOpen(); }

std::optional<sf::Event> GameWindow::pollScreenEvent() {
  while (std::optional<sf::Event> windowEvent = window_.pollEvent()) {
    if (windowEvent->is<sf::Event::Closed>()) {
      window_.close();
      return std::nullopt;
    }
    if (const auto* resized = windowEvent->getIf<sf::Event::Resized>()) {
      showWholePlayfield(resized->size);
    } else {
      return windowEvent;
    }
  }
  return std::nullopt;
}

void GameWindow::clear() { window_.clear(); }

sf::RenderTarget& GameWindow::renderTarget() { return window_; }

void GameWindow::display() { window_.display(); }

const WindowSizeSelection& GameWindow::windowSizeSelection() const {
  return windowSizeSelection_;
}

bool GameWindow::selectWindowSize(std::size_t index) {
  if (!windowSizeSelection_.select(index)) {
    return false;
  }
  applySelectedWindowSize();
  return true;
}

sf::Vector2f GameWindow::playfieldPointAt(sf::Vector2i pixel) const {
  return window_.mapPixelToCoords(pixel);
}

void GameWindow::showWholePlayfield(sf::Vector2u windowSize) {
  const PlayfieldViewport viewport =
      fitPlayfieldInWindow(windowSize.x, windowSize.y);
  sf::View view(sf::FloatRect({0.0F, 0.0F},
                              {game::PLAYFIELD_WIDTH, game::PLAYFIELD_HEIGHT}));
  view.setViewport(sf::FloatRect({viewport.left, viewport.top},
                                 {viewport.width, viewport.height}));
  window_.setView(view);
}

void GameWindow::applySelectedWindowSize() {
  const PixelSize size = windowSizeSelection_.selected();
  const PixelPosition position = windowSizeSelection_.centeredPosition();
  window_.setSize({size.width, size.height});
  window_.setPosition({position.x, position.y});
  showWholePlayfield(window_.getSize());
}

}  // namespace rtype::client
