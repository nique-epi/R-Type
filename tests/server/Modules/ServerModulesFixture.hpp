#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "CurrentDirectoryChange.hpp"
#include "LaunchOptions.hpp"
#include "MessageType.hpp"
#include "ServerModuleFixture.hpp"
#include "StopRequest.hpp"
#include "TemporaryFolder.hpp"
#include "UdpTestClient.hpp"

/** @brief The base of the tests of the whole server: the launch options of a
 * server on a free port, an empty folder as the working folder (the server
 * finds its assets from its executable, next to which the build copies the
 * waves file), and a client that can join. */
class ServerModulesFixture : public ServerModuleFixture {
 public:
  ServerModulesFixture();

 protected:
  /** @brief Has the client join the server through the whole handshake.
   * @returns The session token the server gave it. */
  [[nodiscard]] std::uint64_t joinWithTheClient();

  /** @returns The datagrams of @p received that hold a message of @p type,
   * oldest first. */
  [[nodiscard]] static std::vector<ReceivedAt> holding(
      const std::vector<ReceivedAt>& received,
      rtype::network::MessageType type);

  [[nodiscard]] std::uint16_t port() const;
  [[nodiscard]] rtype::server::StopRequest& stopRequest();
  [[nodiscard]] UdpTestClient& client();
  [[nodiscard]] const rtype::server::LaunchOptions& launchOptions() const;

 private:
  std::uint16_t port_;
  rtype::server::StopRequest stopRequest_;
  UdpTestClient client_;
  TemporaryFolder workingFolder_;
  CurrentDirectoryChange workingFolderChange_;
  std::string portText_;
  std::vector<std::string_view> arguments_;
  rtype::server::LaunchOptions launchOptions_;
};
