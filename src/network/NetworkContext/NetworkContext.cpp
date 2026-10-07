#include "NetworkContext.hpp"

namespace rtype::network {

void NetworkContext::run() { ioContext_.run(); }

void NetworkContext::stop() { ioContext_.stop(); }

asio::io_context& NetworkContext::ioContext() { return ioContext_; }

}  // namespace rtype::network
