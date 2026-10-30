#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
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
   * Without `-p` the port is DEFAULT_SERVER_PORT. The last `-p` wins, and so
   * does the last `--seed` and the last `--state-rate`.
   *
   * @throws engine::UnknownLaunchArgumentException for any other argument,
   *         `-h` included: the server takes no address.
   * @throws engine::MissingLaunchOptionValueException when `-p` is the last
   *         argument.
   * @throws network::InvalidPortException when the value of `-p` is not a
   *         port from LOWEST_PORT to HIGHEST_PORT.
   * @throws engine::InvalidLaunchOptionValueException when the value of
   *         `--seed` is not a whole number from 0 to 4294967295, or the value
   *         of `--state-rate` is not a whole number from
   *         LOWEST_WORLD_STATES_PER_SECOND to HIGHEST_WORLD_STATES_PER_SECOND.
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
   * @returns the seed given after `--seed`, nothing when it was not given: the
   * server then draws one.
   */
  [[nodiscard]] std::optional<std::uint32_t> seed() const;

  /**
   * @returns the number of world states each player is sent per second, the
   * value given after `--state-rate`; DEFAULT_WORLD_STATES_PER_SECOND when it
   * was not given.
   */
  [[nodiscard]] std::size_t stateRate() const;

  /**
   * @returns whether `--help` was given: the server then prints its usage
   *          and starts nothing.
   */
  [[nodiscard]] bool helpRequested() const;

 private:
  LaunchOptions(std::uint16_t port, bool helpRequested);

  std::uint16_t port_;
  bool helpRequested_;
  std::optional<std::uint32_t> seed_;
  std::size_t stateRate_;
};

}  // namespace rtype::server
