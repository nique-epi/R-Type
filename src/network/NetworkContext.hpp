#pragma once

#include <asio/io_context.hpp>

namespace rtype::network {

/**
 * @brief Owns the Asio event loop used by every network component.
 */
class NetworkContext {
 public:
  /**
   * @brief Runs the event loop until it has no work left or stop() is called.
   */
  void run();
  void stop();

  asio::io_context& ioContext();

 private:
  asio::io_context ioContext_;
};

}  // namespace rtype::network
