#include "GameEndAnnouncerFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameEndAnnouncerTestConstants.hpp"
#include "GameEnded.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

ConnectedPlayer& GameEndAnnouncerFixture::join(std::size_t index) {
  return *roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                      SESSION_TOKEN);
}

void GameEndAnnouncerFixture::endTheGame() {
  announcer_ = std::make_unique<GameEndAnnouncer>(
      roster_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      });
}

void GameEndAnnouncerFixture::runFinishedTicks(std::size_t count) {
  ASSERT_NE(announcer_, nullptr);
  for (std::size_t tick = 0; tick < count; ++tick) {
    announcer_->onFinishedTick();
  }
}

std::vector<network::OutgoingDatagram> GameEndAnnouncerFixture::sentTo(
    std::size_t index) const {
  return testing::sentTo(sent_, playerEndpoint(index));
}

network::GameEnded GameEndAnnouncerFixture::resultOfBothPlayers() {
  return network::GameEnded{
      .players = {network::PlayerScore{.playerNumber = FIRST_JOINED_NUMBER,
                                       .score = FIRST_PLAYER_FINAL_SCORE},
                  network::PlayerScore{.playerNumber = SECOND_JOINED_NUMBER,
                                       .score = SECOND_PLAYER_FINAL_SCORE}}};
}

}  // namespace rtype::server::testing
