#include "AdminConsoleCommandsFixture.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "AdminConsoleCommandsTestConstants.hpp"
#include "AdminConsoleParts.hpp"
#include "CommandResult.hpp"
#include "GameStatus.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingChildState.hpp"
#include "RejectionsFixtures.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"

AdminConsoleCommandsFixture::AdminConsoleCommandsFixture()
    : commands_(rtype::server::AdminConsoleParts{.games = registry_,
                                                 .sender = sender_,
                                                 .traffic = traffic_,
                                                 .rejections = rejections_,
                                                 .measures = measures_,
                                                 .interpreter = interpreter_,
                                                 .output = output_}) {
  commands_.registerInto(interpreter_);
}

rtype::engine::CommandResult AdminConsoleCommandsFixture::run(
    std::string_view line) {
  return interpreter_.execute(line);
}

void AdminConsoleCommandsFixture::openGameThree() {
  registry_.open(
      openingOf(GAME_THREE, std::make_shared<RecordingChildState>()));
}

void AdminConsoleCommandsFixture::openGameThreeWithItsPilots() {
  openReporting(registry_, GAME_THREE,
                statusWithPlayers(rtype::network::GameStatusState::InProgress,
                                  REPORTED_PILOT_COUNT));
}

void AdminConsoleCommandsFixture::rejectTwoDatagrams() {
  recordStranger(rejections_, 0);
  recordStranger(rejections_, 0);
}

void AdminConsoleCommandsFixture::registerAgain() {
  commands_.registerInto(interpreter_);
}

const std::vector<rtype::network::OutgoingDatagram>&
AdminConsoleCommandsFixture::sent() const {
  return sender_.sent();
}

std::vector<std::string> AdminConsoleCommandsFixture::lines() const {
  return output_.lines();
}
