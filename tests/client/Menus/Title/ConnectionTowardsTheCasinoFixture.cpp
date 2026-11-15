#include "ConnectionTowardsTheCasinoFixture.hpp"
#include <memory>
#include "ConnectionGoal.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenConstants.hpp"
#include "ConnectionScreenFixtures.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ServerChoice.hpp"

using rtype::client::ACCEPTED_STATUS_DURATION;
using rtype::client::ConnectionGoal;
using rtype::client::ConnectionScreen;
using rtype::client::ConnectionScreenParts;
using rtype::client::ServerChoice;
using rtype::client::testing::ACCEPTED_AS_FIRST_PLAYER;

ConnectionTowardsTheCasinoFixture::ConnectionTowardsTheCasinoFixture()
    : choice_(launchChoice()) {
  choice_.towardsTheCasino = true;
  screen_ = std::make_unique<ConnectionScreen>(
      ConnectionScreenParts{.navigator = navigator_,
                            .sounds = sounds_,
                            .connection = connection_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .choice = choice_,
                            .goal = ConnectionGoal::WaitingRoom},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void ConnectionTowardsTheCasinoFixture::accept() {
  connection_.setStatus(ACCEPTED_AS_FIRST_PLAYER);
  screen_->update(ACCEPTED_STATUS_DURATION);
}

const RecordingNavigator& ConnectionTowardsTheCasinoFixture::navigator() const {
  return navigator_;
}

const ServerChoice& ConnectionTowardsTheCasinoFixture::choice() const {
  return choice_;
}
