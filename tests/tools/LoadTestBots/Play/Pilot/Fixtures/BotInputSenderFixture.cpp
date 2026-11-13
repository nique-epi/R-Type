#include "BotInputSenderFixture.hpp"
#include <cstddef>
#include <cstdint>
#include "BotInputSenderTestConstants.hpp"
#include "EntityMessage.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::tools::testing {

void BotInputSenderFixture::setGameStarted(bool started) {
  room_.setGameStarted(started);
}

std::size_t BotInputSenderFixture::advanceFor(std::size_t frames,
                                              engine::Duration frame) {
  std::size_t sent = 0;
  for (std::size_t count = 0; count < frames; ++count) {
    if (sender_.advance(frame, sight_)) {
      ++sent;
    }
  }
  return sent;
}

const RecordingPlayerInputLink& BotInputSenderFixture::link() const {
  return link_;
}

void BotInputSenderFixture::confirmFrom(const client::EntityMessage& message,
                                        std::uint8_t playerNumber) {
  sender_.confirm(message, playerNumber);
}

client::EntityMessage BotInputSenderFixture::stateAcknowledgingFor(
    std::uint8_t playerNumber) {
  return network::WorldState{
      .tick = 1,
      .players = {network::PlayerState{
          .playerNumber = playerNumber,
          .shipEntityId = SHIP_ENTITY_ID,
          .acknowledgedInput = ACKNOWLEDGED_INPUT}},
      .entities = {network::EntityState{.entityId = SHIP_ENTITY_ID}}};
}

}  // namespace rtype::tools::testing
