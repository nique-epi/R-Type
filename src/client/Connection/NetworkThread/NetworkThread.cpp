#include "NetworkThread.hpp"
#include <exception>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include "BoundedQueue.hpp"
#include "ConnectionConstants.hpp"
#include "DatagramRelay.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "UdpSocket.hpp"

namespace rtype::client {

NetworkThread::NetworkThread(const network::Endpoint& localEndpoint)
    : socket_(network_, localEndpoint),
      localEndpoint_(socket_.localEndpoint()),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY),
      relay_(socket_, incoming_, outgoing_) {
  relay_.startReceiving();
  thread_ = std::jthread{[this] { runNetwork(); }};
}

NetworkThread::~NetworkThread() {
  network_.post([this] {
    relay_.sendWaiting();
    network_.stop();
  });
}

void NetworkThread::send(network::OutgoingDatagram datagram) {
  outgoing_.push(std::move(datagram));
  network_.post([this] { relay_.sendWaiting(); });
}

void NetworkThread::drainReceived(
    std::vector<network::IncomingDatagram>& destination) {
  rethrowNetworkError();
  incoming_.drainInto(destination);
}

network::Endpoint NetworkThread::localEndpoint() const {
  return localEndpoint_;
}

void NetworkThread::runNetwork() {
  try {
    network_.run();
  } catch (...) {
    const std::scoped_lock lock{networkErrorMutex_};
    networkError_ = std::current_exception();
  }
}

void NetworkThread::rethrowNetworkError() const {
  const std::scoped_lock lock{networkErrorMutex_};
  if (networkError_) {
    std::rethrow_exception(networkError_);
  }
}

}  // namespace rtype::client
