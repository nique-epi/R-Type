#pragma once

#include <span>
#include <string>
#include <string_view>
#include "Endpoint.hpp"

namespace rtype::client {

/**
 * @brief What the client is told at launch: the address and the port of the
 * server to join, or a request for its usage.
 *
 * Nothing joins the server yet: the client only checks them and logs them.
 */
class LaunchOptions {
 public:
  /**
   * @brief Reads `-h <address>`, `-p <port>` and `--help` from @p arguments,
   * the arguments the logging options left (see logging::LogLaunchOptions).
   *
   * Without `-h` the address is DEFAULT_SERVER_ADDRESS, without `-p` the
   * port is DEFAULT_SERVER_PORT. The last of each option wins.
   *
   * @throws UnknownLaunchArgumentException for any other argument.
   * @throws MissingLaunchOptionValueException when `-h` or `-p` is the last
   *         argument.
   * @throws network::InvalidIpv4AddressException when the value of `-h` is
   *         not an IPv4 address written as four numbers: the server only
   *         listens over IPv4, and host names are not resolved.
   * @throws network::InvalidPortException when the value of `-p` is not a
   *         port from LOWEST_PORT to HIGHEST_PORT.
   */
  [[nodiscard]] static LaunchOptions read(
      std::span<const std::string_view> arguments);

  /**
   * @returns what `--help` prints: every option of the client, the logging
   *          ones included, with its default.
   */
  [[nodiscard]] static std::string usage();

  /**
   * @returns what the client prints when it cannot read its arguments: its
   *          name and @p message on one line, then how to see the usage.
   */
  [[nodiscard]] static std::string usageError(std::string_view message);

  /** @returns the address and the port of the server to join. */
  [[nodiscard]] const network::Endpoint& serverEndpoint() const;

  /**
   * @returns whether `--help` was given: the client then prints its usage
   *          and opens no window.
   */
  [[nodiscard]] bool helpRequested() const;

 private:
  LaunchOptions(network::Endpoint serverEndpoint, bool helpRequested);

  network::Endpoint serverEndpoint_;
  bool helpRequested_;
};

}  // namespace rtype::client
