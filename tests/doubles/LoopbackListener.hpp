#pragma once

#include <httplib.h>
#include <string>
#include <thread>

/**
 * @brief Serves an httplib::Server whose routes are set, on a port of the
 * loopback the system chooses and on a thread of its own, from its
 * construction to its destruction: the listening part every fake HTTP server
 * of the tests shares.
 */
class LoopbackListener {
 public:
  /** @brief Binds @p server to a free port of @p host and listens; returns
   * once it is ready. @p server must outlive the listener. */
  LoopbackListener(httplib::Server& server, const std::string& host);
  /** @brief Stops the server and waits for its thread. */
  ~LoopbackListener();
  LoopbackListener(const LoopbackListener&) = delete;
  LoopbackListener& operator=(const LoopbackListener&) = delete;
  LoopbackListener(LoopbackListener&&) = delete;
  LoopbackListener& operator=(LoopbackListener&&) = delete;

  /** @returns The port it listens on. */
  [[nodiscard]] int port() const;

 private:
  httplib::Server* server_;
  int port_{};
  std::thread listener_;
};
