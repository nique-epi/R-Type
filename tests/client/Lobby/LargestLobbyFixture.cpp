#include "LargestLobbyFixture.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <string>
#include "LobbyPlace.hpp"
#include "LobbyScreenFixture.hpp"
#include "LobbyScreenFixtures.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ScreenTestConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"

using rtype::client::testing::FIRST_BAY_MIDDLE_COLUMN;
using rtype::client::testing::LARGEST_TEXT;
using rtype::client::testing::LONGEST_NICKNAME;
using rtype::client::testing::OTHER_NUMBER;
using rtype::client::testing::OWN_NUMBER;

LargestLobbyFixture::LargestLobbyFixture()
    : LobbyScreenFixture(rtype::client::LobbyPlace::AfterConnection,
                         LARGEST_TEXT) {}

void LargestLobbyFixture::listFourLongestPilots() {
  list({pilot(OWN_NUMBER, LONGEST_NICKNAME, false),
        pilot(OTHER_NUMBER - 1, LONGEST_NICKNAME, true),
        pilot(OTHER_NUMBER, LONGEST_NICKNAME, false),
        pilot(OTHER_NUMBER + 1, LONGEST_NICKNAME, true)});
}

void LargestLobbyFixture::turnTheWheel(float notches) {
  screen().handleEvent(sf::Event::MouseWheelScrolled{
      .wheel = sf::Mouse::Wheel::Vertical, .delta = notches, .position = {}});
  screen().update(ONE_FRAME);
}

std::string LargestLobbyFixture::nicknameOfTheFirstBay() const {
  std::string nickname;
  for (const RecordedWriting& writing : canvas().writings) {
    if (writing.anchor.x == FIRST_BAY_MIDDLE_COLUMN &&
        writing.style.font == rtype::client::TextFont::Small &&
        writing.style.color == rtype::client::DIALOG_MESSAGE_COLOR) {
      nickname += writing.text;
    }
  }
  return nickname;
}
