#include "NetworkThread.hpp"
#include <exception>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include "BoundedQueue.hpp"
#include "ConnectionConstants.hpp"
#include "DatagramRelay.hpp"
#include "DatagramSocketOpening.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client {

NetworkThread::NetworkThread(network::Endpoint localEndpoint,
                             const network::NetworkConditions& conditions)
    : listeningEndpoint_(std::move(localEndpoint)),
      conditions_(conditions),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY) {}

NetworkThread::~NetworkThread() { close(); }

void NetworkThread::open() {
  if (socket_) {
    return;
  }
  socket_ =
      network::openDatagramSocket(network_, listeningEndpoint_, conditions_);
  try {
    startNetwork(*socket_);
  } catch (...) {
    relay_.reset();
    socket_.reset();
    throw;
  }
}

void NetworkThread::close() {
  if (!thread_.joinable()) {
    return;
  }
  network_.post([this] {
    relay_->sendWaiting();
    network_.stop();
  });
  thread_.join();
  relay_.reset();
  socket_.reset();
}

void NetworkThread::send(network::OutgoingDatagram datagram) {
  outgoing_.push(std::move(datagram));
  network_.post([this] { relay_->sendWaiting(); });
}

void NetworkThread::drainReceived(
    std::vector<network::IncomingDatagram>& destination) {
  rethrowNetworkError();
  incoming_.drainInto(destination);
}

network::Endpoint NetworkThread::localEndpoint() const {
  return localEndpoint_;
}

void NetworkThread::startNetwork(network::IDatagramSocket& socket) {
  localEndpoint_ = socket.localEndpoint();
  relay_.emplace(socket, incoming_, outgoing_).startReceiving();
  thread_ = std::jthread{[this] { runNetwork(); }};
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
