#include "NetworkContext.hpp"
#include <asio/executor_work_guard.hpp>
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

void NetworkContext::runUntilStopped() {
  const auto keepRunning = asio::make_work_guard(*ioContext_);
  ioContext_->run();
}

void NetworkContext::stop() { ioContext_->stop(); }

void NetworkContext::post(std::function<void()> task) {
  asio::post(*ioContext_, std::move(task));
}

asio::io_context& NetworkContext::ioContext() { return *ioContext_; }

}  // namespace rtype::network
