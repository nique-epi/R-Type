#include "RowControl.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

void RowControl::paintOverlay([[maybe_unused]] IInterfaceCanvas& canvas,
                              [[maybe_unused]] sf::IntRect area) const {}

bool RowControl::isActivatable() const { return false; }

void RowControl::activate() {}

bool RowControl::holdsInput() const { return false; }

bool RowControl::keyPressed([[maybe_unused]] const sf::Event::KeyPressed& key) {
  return false;
}

void RowControl::keyReleased(
    [[maybe_unused]] const sf::Event::KeyReleased& key) {}

bool RowControl::pointerPressed([[maybe_unused]] sf::Vector2i pixel,
                                [[maybe_unused]] sf::IntRect area) {
  return false;
}

void RowControl::pointerMoved([[maybe_unused]] sf::Vector2i pixel,
                              [[maybe_unused]] sf::IntRect area) {}

void RowControl::pointerReleased([[maybe_unused]] sf::Vector2i pixel,
                                 [[maybe_unused]] sf::IntRect area) {}

void RowControl::cancelInput() {}

}  // namespace rtype::client
