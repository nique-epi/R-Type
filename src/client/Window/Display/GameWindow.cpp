#include "GameWindow.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "BorderlessScreen.hpp"
#include "DisplayMode.hpp"
#include "PixelPosition.hpp"
#include "PixelSize.hpp"
#include "PlayfieldConstants.hpp"
#include "PlayfieldViewport.hpp"
#include "WindowConstants.hpp"
#include "WindowFrame.hpp"
#include "WindowLayout.hpp"

namespace rtype::client {

namespace {

/** @brief What SFML needs to open a window: its video mode, its style, and
 * whether it is fullscreen. */
struct WindowCreation {
  sf::VideoMode mode;
  std::uint32_t style{};
  sf::State state{};
};

/** @returns The size of the desktop the window opens on. */
PixelSize desktopSize() {
  const sf::Vector2u size = sf::VideoMode::getDesktopMode().size;
  return {.width = size.x, .height = size.y};
}

/**
 * @returns How to open the window the layout describes.
 *
 * Borderless, a window without border as large as the desktop, or, where
 * borderlessUsesDesktopFullscreen() asks for it, SFML's fullscreen at the video
 * mode of the desktop: on macOS, a window without border above the menu bar and
 * the Dock; on Linux, a window the window manager is asked to show fullscreen.
 * Neither changes the resolution of the screen.
 */
WindowCreation creationOf(const WindowLayout& layout) {
  const PixelSize size = layout.frame().size;
  const sf::VideoMode mode({size.width, size.height});
  if (layout.displayMode() == DisplayMode::Windowed) {
    return {.mode = mode,
            .style = sf::Style::Titlebar | sf::Style::Close,
            .state = sf::State::Windowed};
  }
  if (borderlessUsesDesktopFullscreen()) {
    return {.mode = sf::VideoMode::getDesktopMode(),
            .style = sf::Style::None,
            .state = sf::State::Fullscreen};
  }
  return {.mode = mode, .style = sf::Style::None, .state = sf::State::Windowed};
}

/**
 * @returns The view that shows the whole playfield, undistorted and centered,
 * in a window of this size.
 *
 * Set again every time the size of the window changes, SFML keeping the
 * previous view otherwise, which stretches the playfield over the new size; and
 * every time the window opens, SFML then giving it its default view.
 */
sf::View playfieldViewOf(sf::Vector2u windowSize) {
  const PlayfieldViewport viewport =
      fitPlayfieldInWindow(windowSize.x, windowSize.y);
  sf::View view(sf::FloatRect({0.0F, 0.0F},
                              {game::PLAYFIELD_WIDTH, game::PLAYFIELD_HEIGHT}));
  view.setViewport(sf::FloatRect({viewport.left, viewport.top},
                                 {viewport.width, viewport.height}));
  return view;
}

/** @brief Puts the window at this position, and makes it show the whole
 * playfield at its current size. */
void showAt(sf::RenderWindow& window, PixelPosition position) {
  window.setPosition({position.x, position.y});
  window.setView(playfieldViewOf(window.getSize()));
}

/**
 * @brief Opens the window as the layout describes it, closing it first when it
 * is open, then puts it in place.
 *
 * SFML gives a window it opens no frame limit, so the limit is set again.
 */
void openWindow(sf::RenderWindow& window, const WindowLayout& layout) {
  const WindowCreation creation = creationOf(layout);
  window.create(creation.mode, WINDOW_TITLE, creation.style, creation.state);
  window.setFramerateLimit(FRAMES_PER_SECOND_LIMIT);
  showAt(window, layout.frame().position);
}

/** @brief Gives the open window the size and the position of this frame. */
void resizeWindow(sf::RenderWindow& window, const WindowFrame& frame) {
  window.setSize({frame.size.width, frame.size.height});
  showAt(window, frame.position);
}

}  // namespace

GameWindow::GameWindow()
    : layout_(desktopSize(), DisplayMode::Windowed, std::nullopt) {}

void GameWindow::open(std::optional<PixelSize> preferredSize,
                      DisplayMode displayMode) {
  layout_ = WindowLayout(desktopSize(), displayMode, preferredSize);
  openWindow(window_, layout_);
}

void GameWindow::close() { window_.close(); }

bool GameWindow::isOpen() const { return window_.isOpen(); }

std::optional<sf::Event> GameWindow::pollScreenEvent() {
  while (std::optional<sf::Event> windowEvent = window_.pollEvent()) {
    if (windowEvent->is<sf::Event::Closed>()) {
      window_.close();
      return std::nullopt;
    }
    if (const auto* resized = windowEvent->getIf<sf::Event::Resized>()) {
      window_.setView(playfieldViewOf(resized->size));
    } else {
      return windowEvent;
    }
  }
  return std::nullopt;
}

sf::RenderTarget& GameWindow::renderTarget() { return window_; }

void GameWindow::display() { window_.display(); }

const WindowLayout& GameWindow::windowLayout() const { return layout_; }

bool GameWindow::selectWindowSize(std::size_t index) {
  if (!layout_.selectSize(index)) {
    return false;
  }
  resizeWindow(window_, layout_.frame());
  return true;
}

bool GameWindow::selectDisplayMode(DisplayMode mode) {
  if (!layout_.selectDisplayMode(mode)) {
    return false;
  }
  openWindow(window_, layout_);
  return true;
}

sf::Vector2f GameWindow::playfieldPointAt(sf::Vector2i pixel) const {
  return window_.mapPixelToCoords(pixel);
}

}  // namespace rtype::client
