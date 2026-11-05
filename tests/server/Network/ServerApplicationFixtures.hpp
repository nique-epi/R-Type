#pragma once

#include <atomic>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"
#include "TickHandler.hpp"

/**
 * @brief What a test tick handler throws to make the server stop.
 */
class TickFailure final : public std::exception {};

/** @returns A handler that reads nothing of what a socket receives. */
[[nodiscard]] rtype::network::DatagramHandler ignoringWhatItReceives();

/** @brief What a tick handler of admittingFirstRequester() writes down, from
 * the simulation thread, for the test thread. */
struct AdmissionWatch {
  std::promise<void> admission;
  std::promise<rtype::network::IncomingDatagram> firstDatagram;
  std::atomic<bool> admissionDone{false};
  std::atomic<bool> delivered{false};
  /** @brief The server the handler admits on; set before it opens. */
  rtype::server::ServerApplication* server{nullptr};
};

/**
 * @returns A tick handler that admits, under SESSION_TOKEN, the sender of the
 * first connection request it is handed, as the connection acceptor does, and
 * hands @p watch the first datagram of a player it is handed. @p watch must
 * outlive the handler.
 */
[[nodiscard]] rtype::server::TickHandler admittingFirstRequester(
    AdmissionWatch& watch);

/** @brief The datagram a tick handler of queueingOnce() queues, and whether
 * it did. */
struct QueuedOnce {
  rtype::network::OutgoingDatagram datagram;
  std::atomic<bool> queued{false};
  /** @brief The server the handler queues on; set before it opens. */
  rtype::server::ServerApplication* server{nullptr};
};

/** @returns A tick handler that queues the datagram of @p once on its server
 * at the first tick only. @p once must outlive the handler. */
[[nodiscard]] rtype::server::TickHandler queueingOnce(QueuedOnce& once);

/** @brief What the ticks of a server were handed in
 * sortReadyStateThenRequest().
 */
struct SortedReadyState {
  bool requestReachedATick{};
  bool readyStateReachedATick{};
};

/**
 * @brief Runs a server on the loopback address, lets @p prepare admit or forget
 * the address of a client before the server starts, then has that client send
 * @p readyState, then a connection request.
 * @returns Whether a tick was handed the request within THREAD_TIMEOUT, and
 * whether any tick was handed @p readyState, among the datagrams of the players
 * or the requests.
 */
[[nodiscard]] SortedReadyState sortReadyStateThenRequest(
    const std::function<void(rtype::server::ServerApplication&,
                             const rtype::network::Endpoint&)>& prepare,
    const std::vector<std::byte>& readyState);

/**
 * @brief Runs a server on the loopback address. One client sends it
 * FLOOD_REQUESTS_AROUND_A_CLIENT connection requests, a second client one
 * request, then the first client FLOOD_REQUESTS_AROUND_A_CLIENT more, all at
 * once.
 * @returns Whether a tick was handed the request of the second client within
 * THREAD_TIMEOUT.
 */
[[nodiscard]] bool requestOfAClientReachesATickThroughAFlood();
