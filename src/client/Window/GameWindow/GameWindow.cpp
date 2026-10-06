#include "GameWindow.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <optional>
#include "PlayfieldConstants.hpp"
#include "PlayfieldViewport.hpp"
#include "WindowColors.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

GameWindow::GameWindow()
    : window_(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), WINDOW_TITLE),
      playfieldBackground_({game::PLAYFIELD_WIDTH, game::PLAYFIELD_HEIGHT}) {
  window_.setFramerateLimit(FRAMES_PER_SECOND_LIMIT);
  playfieldBackground_.setFillColor(PLAYFIELD_BACKGROUND_COLOR);
  showWholePlayfield(window_.getSize());
}

void GameWindow::run() {
  while (window_.isOpen()) {
    handleEvents();
    render();
  }
}

void GameWindow::handleEvents() {
  while (const std::optional windowEvent = window_.pollEvent()) {
    if (windowEvent->is<sf::Event::Closed>()) {
      window_.close();
    } else if (const auto* resized = windowEvent->getIf<sf::Event::Resized>()) {
      showWholePlayfield(resized->size);
    }
  }
}

void GameWindow::render() {
  window_.clear();
  window_.draw(playfieldBackground_);
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

}  // namespace rtype::client
