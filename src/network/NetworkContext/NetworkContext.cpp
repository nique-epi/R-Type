#include "NetworkContext.hpp"
#include <asio/io_context.hpp>
#include <memory>

namespace rtype::network {

NetworkContext::NetworkContext()
    : ioContext_(std::make_unique<asio::io_context>()) {}

NetworkContext::~NetworkContext() = default;

void NetworkContext::run() { ioContext_->run(); }

void NetworkContext::stop() { ioContext_->stop(); }

asio::io_context& NetworkContext::ioContext() { return *ioContext_; }

}  // namespace rtype::network
