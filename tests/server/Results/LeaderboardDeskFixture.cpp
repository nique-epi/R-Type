#include "LeaderboardDeskFixture.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameResultKeeperTestConstants.hpp"
#include "LeaderboardSamples.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingResultsArchive.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

LeaderboardDeskFixture::LeaderboardDeskFixture()
    : desk_(roster_, archive_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

ConnectedPlayer& LeaderboardDeskFixture::seat(std::size_t index) {
  return *roster_.add(playerEndpoint(index), std::string{NICKNAMES.at(index)},
                      SESSION_TOKEN + index);
}

void LeaderboardDeskFixture::leave(std::size_t index) {
  const std::optional<ConnectedPlayer> departed =
      roster_.remove(playerEndpoint(index));
  if (departed.has_value()) {
    desk_.onPlayerClosing(*departed);
  }
}

ConnectedPlayer& LeaderboardDeskFixture::seatAgainUnderANewToken(
    std::size_t index) {
  return *roster_.add(playerEndpoint(index), std::string{NICKNAMES.at(index)},
                      OTHER_SESSION_TOKEN);
}

void LeaderboardDeskFixture::ask(const ConnectedPlayer& player) {
  desk_.onAsked(player, someLeaderboardRequest());
}

void LeaderboardDeskFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    desk_.onTick();
  }
}

RecordingResultsArchive& LeaderboardDeskFixture::archive() { return archive_; }

const std::vector<network::OutgoingDatagram>& LeaderboardDeskFixture::sent()
    const {
  return sent_;
}

}  // namespace rtype::server::testing
