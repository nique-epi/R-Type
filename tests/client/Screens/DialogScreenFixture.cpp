#include "DialogScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "DialogScreen.hpp"
#include "DialogScreenTestConstants.hpp"
#include "KeyPress.hpp"
#include "MenuBuilder.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::DialogScreen;
using rtype::client::MenuBuilder;
using rtype::client::testing::A_DIALOG_BUTTON;
using rtype::client::testing::A_DIALOG_HEADING;

DialogScreenFixture::DialogScreenFixture()
    : dialog_(
          MenuBuilder(MenuBuilder::Frame::Dialog)
              .heading(A_DIALOG_HEADING)
              .button({.label = A_DIALOG_BUTTON}, [this] { ++activations_; })
              .build(),
          samePlayfieldPoint,
          std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

DialogScreen& DialogScreenFixture::dialog() { return dialog_; }

const InterfaceCanvasRecord& DialogScreenFixture::canvas() const {
  return canvas_;
}

int DialogScreenFixture::activations() const { return activations_; }

void DialogScreenFixture::tap(sf::Keyboard::Key key) {
  dialog_.handleEvent(keyPress(key));
  dialog_.handleEvent(keyRelease(key));
}
