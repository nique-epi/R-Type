#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "LaunchOptions.hpp"
#include "MessageType.hpp"
#include "ServerModuleFixture.hpp"
#include "StopRequest.hpp"
#include "UdpTestClient.hpp"

/** @brief The base of the tests of the whole server: the launch options of a
 * server on a free port, the folder of the repository as the working folder
 * (the server reads assets/waves/waves.toml from there), and a client that
 * can join. */
class ServerModulesFixture : public ServerModuleFixture {
 protected:
  ServerModulesFixture();
  ~ServerModulesFixture() override;

  ServerModulesFixture(const ServerModulesFixture&) = delete;
  ServerModulesFixture& operator=(const ServerModulesFixture&) = delete;
  ServerModulesFixture(ServerModulesFixture&&) = delete;
  ServerModulesFixture& operator=(ServerModulesFixture&&) = delete;

  /** @brief Has the client join the server through the whole handshake.
   * @returns The session token the server gave it. */
  [[nodiscard]] std::uint64_t joinWithTheClient();

  /** @returns The datagrams of @p received that hold a message of @p type,
   * oldest first. */
  [[nodiscard]] static std::vector<ReceivedAt> holding(
      const std::vector<ReceivedAt>& received,
      rtype::network::MessageType type);

  std::uint16_t port_;
  rtype::server::StopRequest stopRequest_;
  UdpTestClient client_;

 private:
  std::filesystem::path previousFolder_;
  std::string portText_;
  std::vector<std::string_view> arguments_;

 protected:
  rtype::server::LaunchOptions launchOptions_;
};
