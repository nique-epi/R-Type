#include "NewGameDialogFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "GameRequest.hpp"
#include "GameRules.hpp"
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "NewGameDialog.hpp"
#include "NewGameDialogTestConstants.hpp"
#include "NewGameMenuParts.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ServerChoice.hpp"
#include "WidgetConstants.hpp"

using rtype::client::GameRequest;
using rtype::client::newGameMenu;
using rtype::client::NewGameMenuParts;
using rtype::client::PRESS_DURATION;
using rtype::client::ServerChoice;
using rtype::client::testing::LISTED_SERVER_ADDRESS;
using rtype::client::testing::LISTED_SERVER_PORT;
using rtype::network::Endpoint;
using rtype::network::GameRules;

void NewGameDialogFixture::open(std::vector<std::string> difficulties) {
  menu_.emplace(
      newGameMenu(NewGameMenuParts{.navigator = navigator_,
                                   .sounds = sounds_,
                                   .choice = choice_,
                                   .server = listedServer(),
                                   .difficulties = std::move(difficulties)}));
}

void NewGameDialogFixture::choose(sf::Keyboard::Key key) {
  if (!menu_.has_value()) {
    return;
  }
  menu_->handleEvent(keyPress(key), samePlayfieldPoint);
  menu_->handleEvent(keyRelease(key), samePlayfieldPoint);
  menu_->advance(PRESS_DURATION);
}

InterfaceCanvasRecord NewGameDialogFixture::painting() const {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  if (menu_.has_value()) {
    menu_->paint(canvas);
  }
  return record;
}

const RecordingNavigator& NewGameDialogFixture::navigator() const {
  return navigator_;
}

const ServerChoice& NewGameDialogFixture::choice() const { return choice_; }

unsigned int NewGameDialogFixture::createdLives() const {
  return choice_.lastRequest.value_or(GameRequest{})
      .rules.value_or(GameRules{})
      .lives;
}

Endpoint NewGameDialogFixture::listedServer() {
  return Endpoint{.address = std::string(LISTED_SERVER_ADDRESS),
                  .port = LISTED_SERVER_PORT};
}
