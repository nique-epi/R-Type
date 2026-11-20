#include "MapBallotFixture.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "LobbyState.hpp"
#include "MapBallot.hpp"
#include "MapBallotTestConstants.hpp"
#include "MapVote.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"
#include "RecordingWaitingRoom.hpp"
#include "SentToServer.hpp"

namespace rtype::client::testing {

void MapBallotFixture::open() {
  ballot_.sessionOpened(
      network::LobbyPlayer{.playerNumber = BALLOT_PLAYER_NUMBER,
                           .ready = false,
                           .nickname = std::string{BALLOT_PLAYER_NICKNAME},
                           .mapVote = network::NO_MAP_VOTE},
      outbox_);
}

MapBallot& MapBallotFixture::ballot() { return ballot_; }

RecordingWaitingRoom& MapBallotFixture::room() { return room_; }

std::vector<std::uint8_t> MapBallotFixture::sentVotes() const {
  std::vector<std::uint8_t> votes;
  for (const SentToServer& sent : outbox_.sent()) {
    if (sent.type == network::MessageType::MapVote) {
      votes.push_back(
          network::decodeMessage<network::MapVote>(sent.payload).map);
    }
  }
  return votes;
}

}  // namespace rtype::client::testing
