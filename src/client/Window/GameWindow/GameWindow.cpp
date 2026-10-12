#include "GameWindow.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <cstddef>
#include <optional>
#include <random>
#include "ITextureSource.hpp"
#include "PixelPosition.hpp"
#include "PixelSize.hpp"
#include "PlayfieldConstants.hpp"
#include "PlayfieldViewport.hpp"
#include "SpriteShowcase.hpp"
#include "TimeConstants.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

PixelSize GameWindow::desktopSize() {
  const sf::Vector2u size = sf::VideoMode::getDesktopMode().size;
  return {.width = size.x, .height = size.y};
}

GameWindow::GameWindow(const sf::Font& windowSizeLabelFont,
                       const ITextureSource& textures)
    : windowSizeSelection_(desktopSize()),
      window_(sf::VideoMode({windowSizeSelection_.selected().width,
                             windowSizeSelection_.selected().height}),
              WINDOW_TITLE, sf::Style::Titlebar | sf::Style::Close),
      background_(std::random_device{}()),
      windowSizeButtons_(windowSizeLabelFont),
      entitySurface_(entityCanvas_.canvas(), textures) {
  window_.setFramerateLimit(FRAMES_PER_SECOND_LIMIT);
  applySelectedWindowSize();
  spawnSpriteShowcase(entities_, components_);
}

void GameWindow::run() {
  previousFrameTime_ = clock_.now();
  while (window_.isOpen()) {
    handleEvents();
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
    }
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
  drawEntities();
  windowSizeButtons_.draw(window_);
  window_.display();
}

void GameWindow::drawEntities() {
  sf::RenderTexture& canvas = entityCanvas_.canvas();
  canvas.clear(sf::Color::Transparent);
  renderSystem_.render(components_, entitySurface_);
  canvas.display();
  entityCanvas_.show(window_);
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
