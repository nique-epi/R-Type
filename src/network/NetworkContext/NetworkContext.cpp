#include "NetworkContext.hpp"
#include <asio/io_context.hpp>
#include <asio/post.hpp>
#include <functional>
#include <memory>
#include <utility>

namespace rtype::network {

NetworkContext::NetworkContext()
    : ioContext_(std::make_unique<asio::io_context>()) {}

NetworkContext::~NetworkContext() = default;

void NetworkContext::run() { ioContext_->run(); }

void NetworkContext::stop() { ioContext_->stop(); }

void NetworkContext::post(std::function<void()> task) {
  asio::post(*ioContext_, std::move(task));
}

asio::io_context& NetworkContext::ioContext() { return *ioContext_; }

}  // namespace rtype::network
