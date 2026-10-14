#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>

namespace rtype::server {

/**
 * @brief What the server is told at launch: the UDP port to listen on, or a
 * request for its usage.
 */
class LaunchOptions {
 public:
  /**
   * @brief Reads `-p <port>` and `--help` from @p arguments, the arguments
   * the logging options left (see logging::LogLaunchOptions).
   *
   * Without `-p` the port is DEFAULT_SERVER_PORT. The last `-p` wins.
   *
   * @throws engine::UnknownLaunchArgumentException for any other argument,
   *         `-h` included: the server takes no address.
   * @throws engine::MissingLaunchOptionValueException when `-p` is the last
   *         argument.
   * @throws network::InvalidPortException when the value of `-p` is not a
   *         port from LOWEST_PORT to HIGHEST_PORT.
   */
  [[nodiscard]] static LaunchOptions read(
      std::span<const std::string_view> arguments);

  /**
   * @returns what `--help` prints: every option of the server, the logging
   *          ones included, with its default.
   */
  [[nodiscard]] static std::string usage();

  /** @returns the UDP port to listen on. */
  [[nodiscard]] std::uint16_t port() const;

  /**
   * @returns whether `--help` was given: the server then prints its usage
   *          and starts nothing.
   */
  [[nodiscard]] bool helpRequested() const;

 private:
  LaunchOptions(std::uint16_t port, bool helpRequested);

  std::uint16_t port_;
  bool helpRequested_;
};

}  // namespace rtype::server
