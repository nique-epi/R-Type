#include "GameWindow.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <optional>
#include "WindowConstants.hpp"

namespace rtype::client {

GameWindow::GameWindow()
    : window_(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), WINDOW_TITLE) {
  window_.setFramerateLimit(FRAMES_PER_SECOND_LIMIT);
}

void GameWindow::run() {
  while (window_.isOpen()) {
    handleEvents();
    render();
  }
}

void GameWindow::handleEvents() {
  // NOLINTNEXTLINE(altera-id-dependent-backward-branch)
  while (const std::optional windowEvent = window_.pollEvent()) {
    if (windowEvent->is<sf::Event::Closed>()) {
      window_.close();
    }
  }
}

void GameWindow::render() {
  window_.clear();
  window_.display();
}

}  // namespace rtype::client
