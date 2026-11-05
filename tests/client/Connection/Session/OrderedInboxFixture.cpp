#include "OrderedInboxFixture.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <vector>
#include "EncodedMessage.hpp"
#include "EntityDestroyed.hpp"
#include "GameEvent.hpp"
#include "Heartbeat.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "OrderedInboxTestConstants.hpp"
#include "OrderedMessage.hpp"

namespace rtype::client::testing {

OrderedInboxFixture::OrderedInboxFixture() {
  dispatcher_.registerHandler<network::GameEvent>(
      [this](const network::MessageOrigin&, const network::GameEvent&) {
        handled_.push_back(network::MessageType::GameEvent);
      });
  dispatcher_.registerHandler<network::EntityDestroyed>(
      [this](const network::MessageOrigin&, const network::EntityDestroyed&) {
        handled_.push_back(network::MessageType::EntityDestroyed);
      });
  dispatcher_.registerHandler<network::Heartbeat>(
      [this](const network::MessageOrigin&, const network::Heartbeat&) {
        handled_.push_back(network::MessageType::Heartbeat);
      });
}

void OrderedInboxFixture::deliverOrdered(std::uint16_t number,
                                         std::uint16_t firstKept) {
  const network::EncodedMessage carried =
      number == FIRST_ORDERED
          ? network::encodedMessage(
                network::GameEvent{.eventNumber = SOME_EVENT_NUMBER,
                                   .kind = network::GameEventKind::PlayerHit,
                                   .playerNumber = HIT_PLAYER_NUMBER,
                                   .entityId = 0})
          : network::encodedMessage(network::EntityDestroyed{
                .entityId = DESTROYED_ENTITY_ID,
                .reason = network::DestructionReason::Killed});
  const network::EncodedMessage envelope =
      network::encodedMessage(network::OrderedMessage{
          .number = number,
          .firstKept = firstKept,
          .innerType = static_cast<std::uint8_t>(carried.type),
          .payload = carried.payload});
  const std::array<network::MessageFrame, 1> frames{
      network::MessageFrame{.type = static_cast<std::uint8_t>(envelope.type),
                            .payload = envelope.payload}};
  dispatcher_.dispatch(network::MessageOrigin{}, frames);
}

void OrderedInboxFixture::deliverHeartbeat() {
  const network::EncodedMessage heartbeat = network::encodedMessage(
      network::Heartbeat{.sentAt = SOME_SENT_AT, .echo = std::nullopt});
  const std::array<network::MessageFrame, 1> frames{
      network::MessageFrame{.type = static_cast<std::uint8_t>(heartbeat.type),
                            .payload = heartbeat.payload}};
  dispatcher_.dispatch(network::MessageOrigin{}, frames);
}

void OrderedInboxFixture::closeSession() { inbox_.sessionClosed(); }

const std::vector<network::MessageType>& OrderedInboxFixture::handled() const {
  return handled_;
}

}  // namespace rtype::client::testing
