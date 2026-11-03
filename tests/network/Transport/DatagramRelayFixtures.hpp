#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramRelay.hpp"
#include "DatagramRelayTestConstants.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingDatagramSocket.hpp"

/**
 * @brief A relay between a recording socket and two queues, all owned
 * together.
 */
struct RelayUnderTest {
  RecordingDatagramSocket socket;
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram> incoming{
      RELAY_QUEUE_CAPACITY};
  rtype::concurrency::BoundedQueue<rtype::network::OutgoingDatagram> outgoing{
      RELAY_QUEUE_CAPACITY};
  rtype::network::DatagramRelay relay{socket, incoming, outgoing};
};

/** @returns The peer of the relay tests, on @p port. */
rtype::network::Endpoint peerOnPort(std::uint16_t port);

/** @returns RELAY_PAYLOAD_SIZE bytes, each RECEIVED_BYTE. */
std::vector<std::byte> receivedBytes();
