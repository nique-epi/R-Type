#include "ScreenInputs.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <string_view>
#include "IScreen.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

void tapKey(rtype::client::IScreen& screen, sf::Keyboard::Key key) {
  screen.handleEvent(keyPress(key));
  screen.handleEvent(keyRelease(key));
}

void typeText(rtype::client::IScreen& screen, std::u32string_view text) {
  for (const char32_t character : text) {
    screen.handleEvent(typedCharacter(character));
  }
}

void eraseCharacters(rtype::client::IScreen& screen, std::size_t characters) {
  for (std::size_t erased = 0; erased < characters; ++erased) {
    screen.handleEvent(keyPress(sf::Keyboard::Key::Backspace));
  }
}

void clickInterfacePixel(rtype::client::IScreen& screen,
                         sf::Vector2i interfacePixel) {
  screen.handleEvent(leftPress(interfacePixel * INTERFACE_PIXEL));
}

void advanceFrameByFrame(rtype::client::IScreen& screen,
                         rtype::engine::Duration duration) {
  while (duration > rtype::engine::Duration::zero()) {
    const rtype::engine::Duration step = std::min(duration, ONE_FRAME);
    screen.update(step);
    duration -= step;
  }
}
