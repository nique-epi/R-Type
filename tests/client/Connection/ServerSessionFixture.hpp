#pragma once

#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "ConnectionPhase.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "EntityMessageQueue.hpp"
#include "GameLoadedReminder.hpp"
#include "IncomingDatagram.hpp"
#include "MessageDispatcher.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "RecordingDatagramLink.hpp"
#include "RoundTripWatch.hpp"
#include "ServerSession.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionParts.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/** @brief A session, its waiting room, its entity message queue and its round
 * trip watch wired as the connection module wires them, over a recording link
 * and a clock that moves with the frames. */
class ServerSessionFixture : public ::testing::Test {
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

  /** @brief Runs one frame of the network stage, as the connection module
   * does: the clock moves, the queue counts the time, then the session reads
   * and sends. */
  void update(engine::Duration elapsed = engine::Duration::zero());

  /** @brief Connects, then has the server accept the attempt, its acceptance
   * carrying SESSION_TOKEN. */
  void connectAndGetAccepted();

  [[nodiscard]] ConnectionPhase phase() const;

  [[nodiscard]] ServerSession& session();

  [[nodiscard]] WaitingRoom& waitingRoom();

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

  /** @returns The entity messages the queue hands over now. */
  [[nodiscard]] std::vector<EntityMessage> drainedEntityMessages();

  /** @returns How many datagrams sent so far hold a message of type
   * Message. */
  template <typename Message>
  [[nodiscard]] std::size_t sentCountOf() const {
    return static_cast<std::size_t>(
        std::ranges::count_if(link_.sent(), holds<Message>));
  }

  /** @returns The sender the link was last told to listen to. */
  [[nodiscard]] const std::optional<network::Endpoint>& listenedTo() const;

  [[nodiscard]] network::PlayerInput lastInput() const;

  [[nodiscard]] network::PacketHeader lastHeader() const;

 private:
  RecordingDatagramLink link_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  WaitingRoom waitingRoom_{dispatcher_, gameLoaded_};
  EntityMessageQueue entityMessages_{dispatcher_, waitingRoom_, gameLoaded_};
  SimulatedClock clock_;
  RoundTripWatch roundTrips_{dispatcher_, clock_};
  ServerSession session_{ServerSessionParts{
      .link = link_,
      .dispatcher = dispatcher_,
      .gameLoaded = gameLoaded_,
      .observers = {waitingRoom_, entityMessages_, gameLoaded_, roundTrips_}}};
};

}  // namespace rtype::client::testing
