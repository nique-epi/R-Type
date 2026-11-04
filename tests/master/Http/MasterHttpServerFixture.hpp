#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <atomic>
#include <cstdint>
#include <string>
#include <string_view>
#include "MasterHttpServer.hpp"

/**
 * @brief An HTTP server of the master on the loopback address, a port the
 * system chooses, and a client that calls it.
 */
class MasterHttpServerFixture : public ::testing::Test {
 protected:
  MasterHttpServerFixture();

  /** @returns The server, to add its routes before openServer(). */
  [[nodiscard]] rtype::master::MasterHttpServer& server();

  /** @brief Opens the server on a port the system chooses. */
  void openServer();

  /** @returns The answer of the server to GET @p path. */
  [[nodiscard]] httplib::Result get(std::string_view path) const;

  /** @returns The answer of the server to GET @p path with @p body. */
  [[nodiscard]] httplib::Result getWithBody(std::string_view path,
                                            const std::string& body) const;

  /** @returns The port the server listens on, once openServer() ran. */
  [[nodiscard]] std::uint16_t port() const;

  /** @returns How many times the server said it stopped on its own. */
  [[nodiscard]] int failureCount() const;

 private:
  std::atomic<int> failureCount_{0};
  rtype::master::MasterHttpServer server_;
  std::uint16_t port_{};
};
