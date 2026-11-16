#include "LoopbackListener.hpp"
#include <httplib.h>
#include <string>
#include <thread>

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
LoopbackListener::LoopbackListener(httplib::Server& server,
                                   const std::string& host)
    : server_(&server), port_(server.bind_to_any_port(host)) {
  listener_ = std::thread{[this] { server_->listen_after_bind(); }};
  server_->wait_until_ready();
}
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)

LoopbackListener::~LoopbackListener() {
  server_->stop();
  listener_.join();
}

int LoopbackListener::port() const { return port_; }
