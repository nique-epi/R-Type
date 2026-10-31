#pragma once

#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectionPhase.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerConnection.hpp"
#include "ServerConnectionFixtures.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/** @brief A connection whose datagrams go through a recording link. */
class ServerConnectionFixture : public ::testing::Test {
 protected:
  void connect();

  /** @brief Delivers a datagram holding @p message, as @p sender sent it. */
  template <typename Message>
  void deliver(const Message& message,
               const network::Endpoint& sender = server(),
               network::PacketHeader header = {}) {
    link_.deliver(network::IncomingDatagram{
        .sender = sender,
        .payload = network::DatagramCodec::encode(header, message)});
  }

  void deliverBytes(std::vector<std::byte> datagram);

  void update(engine::Duration elapsed = engine::Duration::zero());

  /** @brief Connects, then has the server accept the attempt, its acceptance
   * carrying SESSION_TOKEN. */
  void connectAndGetAccepted();

  [[nodiscard]] ConnectionPhase phase() const;

  [[nodiscard]] ServerConnection& connection();

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const;

  [[nodiscard]] network::ConnectionRequest lastRequest() const;

  /** @brief Has the server accept the attempt as ACCEPTED_PLAYER_NUMBER,
   * with SESSION_TOKEN. */
  void accept();

  /** @brief Delivers @p message in a datagram of the server carrying this
   * sequence and SESSION_TOKEN. */
  template <typename Message>
  void deliverInSession(const Message& message, std::uint16_t sequence) {
    deliver(message, server(), sessionHeader(sequence));
  }

  /** @brief Delivers the first part of @p state in a datagram of the session
   * carrying this sequence. */
  void deliverWorldStateInSession(const network::WorldState& state,
                                  std::uint16_t sequence);

  /** @returns The entity messages the connection hands over now. */
  [[nodiscard]] std::vector<EntityMessage> drainedEntityMessages();

  /** @returns How many datagrams sent so far hold a message of type
   * Message. */
  template <typename Message>
  [[nodiscard]] std::size_t sentCountOf() const {
    return static_cast<std::size_t>(
        std::ranges::count_if(link_.sent(), holds<Message>));
  }

  /** @returns A header of the server carrying this sequence and
   * SESSION_TOKEN. */
  static network::PacketHeader sessionHeader(std::uint16_t sequence);

  [[nodiscard]] network::PlayerInput lastInput() const;

  [[nodiscard]] network::PacketHeader lastHeader() const;

 private:
  RecordingDatagramLink link_;
  ServerConnection connection_{link_};
};

}  // namespace rtype::client::testing
