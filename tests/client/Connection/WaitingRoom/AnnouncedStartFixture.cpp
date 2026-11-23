#include "AnnouncedStartFixture.hpp"
#include <array>
#include <cstdint>
#include <string>
#include "AnnouncedStart.hpp"
#include "AnnouncedStartTestConstants.hpp"
#include "EncodedMessage.hpp"
#include "GameStarted.hpp"
#include "LobbyState.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client::testing {

void AnnouncedStartFixture::open() {
  room_.sessionOpened(
      network::LobbyPlayer{.playerNumber = STARTING_PLAYER_NUMBER,
                           .ready = false,
                           .nickname = {}},
      outbox_);
}

void AnnouncedStartFixture::deliver(const network::GameStarted& started) {
  const network::EncodedMessage encoded = network::encodedMessage(started);
  const std::array<network::MessageFrame, 1> frames{
      network::MessageFrame{.type = static_cast<std::uint8_t>(encoded.type),
                            .payload = encoded.payload}};
  dispatcher_.dispatch(network::MessageOrigin{}, frames);
}

WaitingRoom& AnnouncedStartFixture::room() { return room_; }

const AnnouncedStart& AnnouncedStartFixture::start() const { return start_; }

network::GameStarted startInTheCaves() {
  return network::GameStarted{.biome = std::string{ANNOUNCED_BIOME},
                              .scrollPosition = 0.0F,
                              .biomeSince = ANNOUNCED_BIOME_SINCE};
}

}  // namespace rtype::client::testing
