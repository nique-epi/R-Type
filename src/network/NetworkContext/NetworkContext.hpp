#pragma once

#include <memory>

namespace asio {
class io_context;
}  // namespace asio

namespace rtype::network {

/**
 * @brief Owns the Asio event loop used by every network component.
 *
 * Asio is only declared here, never included: the code outside the network
 * module never sees an Asio type.
 */
class NetworkContext {
 public:
  NetworkContext();
  ~NetworkContext();

  NetworkContext(const NetworkContext&) = delete;
  NetworkContext& operator=(const NetworkContext&) = delete;
  NetworkContext(NetworkContext&&) = delete;
  NetworkContext& operator=(NetworkContext&&) = delete;

  /**
   * @brief Runs the event loop until it has no work left or stop() is called.
   */
  void run();
  void stop();

  /**
   * @brief The event loop itself, for the classes of the network module and
   *        their tests: using it requires including Asio, which the other
   *        modules cannot do.
   */
  asio::io_context& ioContext();

 private:
  std::unique_ptr<asio::io_context> ioContext_;
};

}  // namespace rtype::network
