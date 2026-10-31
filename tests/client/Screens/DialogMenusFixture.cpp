#include "DialogMenusFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "ConnectionPhase.hpp"
#include "DialogMenus.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenInputs.hpp"
#include "WidgetConstants.hpp"

using rtype::client::connectionLostMenu;
using rtype::client::ConnectionPhase;
using rtype::client::PRESS_DURATION;
using rtype::client::quitConfirmationMenu;

void DialogMenusFixture::openQuitConfirmation() {
  dialog_.emplace(quitConfirmationMenu(navigator_, sounds_), samePlayfieldPoint,
                  std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void DialogMenusFixture::openConnectionLost(ConnectionPhase ending) {
  dialog_.emplace(connectionLostMenu(navigator_, sounds_, ending),
                  samePlayfieldPoint,
                  std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

const RecordingNavigator& DialogMenusFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& DialogMenusFixture::canvas() const {
  return canvas_;
}

const RecordingSoundPlayer& DialogMenusFixture::sounds() const {
  return sounds_;
}

void DialogMenusFixture::press(sf::Keyboard::Key key) {
  dialog_->handleEvent(keyPress(key));
}

void DialogMenusFixture::tap(sf::Keyboard::Key key) { tapKey(*dialog_, key); }

void DialogMenusFixture::letThePressLast() { dialog_->update(PRESS_DURATION); }
