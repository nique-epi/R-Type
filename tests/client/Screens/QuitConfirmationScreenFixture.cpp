#include "QuitConfirmationScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "QuitConfirmationScreen.hpp"
#include "QuitConfirmationScreenParts.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::QuitConfirmationScreen;
using rtype::client::QuitConfirmationScreenParts;

QuitConfirmationScreenFixture::QuitConfirmationScreenFixture()
    : dialog_(
          QuitConfirmationScreenParts{.navigator = navigator_,
                                      .playfieldPointAt = samePlayfieldPoint},
          std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

const RecordingNavigator& QuitConfirmationScreenFixture::navigator() const {
  return navigator_;
}

QuitConfirmationScreen& QuitConfirmationScreenFixture::dialog() {
  return dialog_;
}

void QuitConfirmationScreenFixture::tap(sf::Keyboard::Key key) {
  dialog_.handleEvent(keyPress(key));
  dialog_.handleEvent(keyRelease(key));
}
