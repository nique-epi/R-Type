#include "ServerLink.hpp"
#include <cstdint>
#include <utility>
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"
#include "ServerException.hpp"

namespace rtype::server {

void ServerLink::open(const network::Endpoint& localEndpoint,
                      ServerApplication::TickHandler handleTick) {
  server_.emplace(localEndpoint, std::move(handleTick));
}

void ServerLink::close() { server_.reset(); }

void ServerLink::run() { openServer().run(); }

void ServerLink::stop() {
  if (server_.has_value()) {
    server_->stop();
  }
}

network::Endpoint ServerLink::localEndpoint() const {
  if (!server_.has_value()) {
    throw ServerLinkClosedException();
  }
  return server_->localEndpoint();
}

void ServerLink::send(network::OutgoingDatagram datagram) {
  openServer().queueDatagram(std::move(datagram));
}

void ServerLink::admit(const network::Endpoint& player,
                       std::uint64_t sessionToken) {
  openServer().admit(player, sessionToken);
}

void ServerLink::forget(const network::Endpoint& player,
                        std::uint64_t sessionToken) {
  openServer().forget(player, sessionToken);
}

void ServerLink::sendQueuedAfterRun() { openServer().sendQueuedAfterRun(); }

ServerApplication& ServerLink::openServer() {
  if (!server_.has_value()) {
    throw ServerLinkClosedException();
  }
  return *server_;
}

}  // namespace rtype::server
