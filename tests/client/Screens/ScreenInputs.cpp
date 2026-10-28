#include "ScreenInputs.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include "IScreen.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

void tapKey(rtype::client::IScreen& screen, sf::Keyboard::Key key) {
  screen.handleEvent(keyPress(key));
  screen.handleEvent(keyRelease(key));
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
