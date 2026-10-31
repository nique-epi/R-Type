#include "ConnectionLostInTheStackFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include "ConnectionPhase.hpp"
#include "DialogMenus.hpp"
#include "DialogScreen.hpp"
#include "IScreen.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "WidgetConstants.hpp"

using rtype::client::connectionLostMenu;
using rtype::client::ConnectionPhase;
using rtype::client::DialogScreen;
using rtype::client::IScreen;
using rtype::client::PRESS_DURATION;
using rtype::client::ScreenId;

ConnectionLostInTheStackFixture::ConnectionLostInTheStackFixture()
    : screens_([this](ScreenId screen) { return build(screen); }) {
  screens_.replaceAll(ScreenId::Title);
  screens_.push(ScreenId::ConnectionLost);
  screens_.applyPendingChanges();
}

void ConnectionLostInTheStackFixture::answerWith(sf::Keyboard::Key key) {
  screens_.handleEvent(keyPress(key));
  screens_.handleEvent(keyRelease(key));
  screens_.applyPendingChanges();
  screens_.update(PRESS_DURATION);
  screens_.applyPendingChanges();
  journal_.clear();
}

void ConnectionLostInTheStackFixture::sendAnEvent() {
  screens_.handleEvent(keyPress(sf::Keyboard::Key::A));
}

const std::vector<JournalEntry>& ConnectionLostInTheStackFixture::journal()
    const {
  return journal_;
}

std::unique_ptr<IScreen> ConnectionLostInTheStackFixture::build(
    ScreenId screen) {
  if (screen == ScreenId::ConnectionLost) {
    return std::make_unique<DialogScreen>(
        connectionLostMenu(screens_, sounds_, ConnectionPhase::Lost),
        samePlayfieldPoint,
        std::make_unique<RecordingInterfaceCanvas>(dialogCanvas_));
  }
  return std::make_unique<RecordingScreen>(screen, journal_);
}
