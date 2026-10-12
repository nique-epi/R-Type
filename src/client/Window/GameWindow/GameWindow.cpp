#include "GameWindow.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include "ActionSet.hpp"
#include "PixelPosition.hpp"
#include "PixelSize.hpp"
#include "PlayfieldConstants.hpp"
#include "PlayfieldViewport.hpp"
#include "TimeConstants.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

PixelSize GameWindow::desktopSize() {
  const sf::Vector2u size = sf::VideoMode::getDesktopMode().size;
  return {.width = size.x, .height = size.y};
}

GameWindow::GameWindow(const sf::Font& windowSizeLabelFont)
    : windowSizeSelection_(desktopSize()),
      window_(sf::VideoMode({windowSizeSelection_.selected().width,
                             windowSizeSelection_.selected().height}),
              WINDOW_TITLE, sf::Style::Titlebar | sf::Style::Close),
      background_(std::random_device{}()),
      windowSizeButtons_(windowSizeLabelFont),
      playerCommands_(window_.hasFocus()) {
  window_.setFramerateLimit(FRAMES_PER_SECOND_LIMIT);
  applySelectedWindowSize();
}

void GameWindow::run() {
  previousFrameTime_ = clock_.now();
  while (window_.isOpen()) {
    handleEvents();
    logHeldActions();
    advanceBackground();
    render();
  }
}

void GameWindow::handleEvents() {
  while (const std::optional windowEvent = window_.pollEvent()) {
    if (windowEvent->is<sf::Event::Closed>()) {
      window_.close();
    } else if (const auto* resized = windowEvent->getIf<sf::Event::Resized>()) {
      showWholePlayfield(resized->size);
    } else if (const auto* pressed =
                   windowEvent->getIf<sf::Event::MouseButtonPressed>()) {
      if (pressed->button == sf::Mouse::Button::Left) {
        selectWindowSizeAt(pressed->position);
      }
    } else if (const auto* keyPressed =
                   windowEvent->getIf<sf::Event::KeyPressed>()) {
      playerCommands_.keyPressed(keyPressed->code);
    } else if (const auto* keyReleased =
                   windowEvent->getIf<sf::Event::KeyReleased>()) {
      playerCommands_.keyReleased(keyReleased->code);
    } else if (windowEvent->is<sf::Event::FocusLost>()) {
      playerCommands_.focusLost();
    } else if (windowEvent->is<sf::Event::FocusGained>()) {
      playerCommands_.focusGained();
    }
  }
}

ActionSet GameWindow::playerActions() const { return playerCommands_.held(); }

void GameWindow::logHeldActions() {
  const std::uint8_t held = playerCommands_.held().bits();
  if (held != loggedActions_) {
    logger_.trace("actions held: ", static_cast<unsigned int>(held));
    loggedActions_ = held;
  }
}

void GameWindow::advanceBackground() {
  const engine::Duration now = clock_.now();
  background_.advance(now - previousFrameTime_);
  previousFrameTime_ = now;
}

void GameWindow::render() {
  window_.clear();
  background_.draw(window_);
  windowSizeButtons_.draw(window_);
  window_.display();
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
  windowSizeButtons_.showState(windowSizeSelection_);
}

void GameWindow::selectWindowSizeAt(sf::Vector2i pixel) {
  const std::optional<std::size_t> index =
      windowSizeButtons_.indexAt(window_.mapPixelToCoords(pixel));
  if (index.has_value() && windowSizeSelection_.select(*index)) {
    applySelectedWindowSize();
  }
}

}  // namespace rtype::client
