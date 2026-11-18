#include "ConnectionScreenTicketFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include "ConnectionScreen.hpp"
#include "ConnectionScreenFixtures.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "FakeAccountSession.hpp"
#include "FakeTicketLink.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingServerConnection.hpp"
#include "ScreenInputs.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionScreenParts;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::SIGNED_IN_PLAYER_ACCOUNT_ID;
using rtype::client::testing::SIGNED_IN_TOKEN;

ConnectionScreenTicketFixture::ConnectionScreenTicketFixture()
    : choice_(launchChoice()),
      screen_(ConnectionScreenParts{.navigator = navigator_,
                                    .sounds = sounds_,
                                    .connection = connection_,
                                    .account = account_,
                                    .tickets = tickets_,
                                    .playfieldPointAt = samePlayfieldPoint,
                                    .choice = choice_},
              std::make_unique<RecordingInterfaceCanvas>(canvas_)) {
  account_.signInAs(SIGNED_IN_PLAYER_ACCOUNT_ID, std::string{SIGNED_IN_TOKEN});
}

void ConnectionScreenTicketFixture::confirm() {
  tapKey(screen_, sf::Keyboard::Key::Enter);
  screen_.update(PRESS_DURATION);
}

void ConnectionScreenTicketFixture::wait(rtype::engine::Duration elapsed) {
  screen_.update(elapsed);
}

RecordingServerConnection& ConnectionScreenTicketFixture::connection() {
  return connection_;
}

FakeTicketLink& ConnectionScreenTicketFixture::tickets() { return tickets_; }

FakeAccountSession& ConnectionScreenTicketFixture::account() {
  return account_;
}

const InterfaceCanvasRecord& ConnectionScreenTicketFixture::canvas() const {
  return canvas_;
}
