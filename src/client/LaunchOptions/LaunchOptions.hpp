#pragma once

#include <span>
#include <string>
#include <string_view>
#include "AudioMode.hpp"
#include "Endpoint.hpp"

namespace rtype::client {

/**
 * @brief What the client is told at launch: the address and the port of the
 * server to join and the nickname of the player, which the connection screen
 * starts with, whether it makes sound, or a request for its usage.
 */
class LaunchOptions {
 public:
  /**
   * @brief Reads `-h <address>`, `-p <port>`, `-n <nickname>`, `--no-sound`
   * and `--help` from @p arguments, the arguments the logging options left (see
   * logging::LogLaunchOptions).
   *
   * Without `-h` the address is DEFAULT_SERVER_ADDRESS, without `-p` the
   * port is DEFAULT_SERVER_PORT, without `-n` the nickname is DEFAULT_NICKNAME,
   * without `--no-sound` the client makes sound.
   * The last of each option wins.
   *
   * @throws engine::UnknownLaunchArgumentException for any other argument.
   * @throws engine::MissingLaunchOptionValueException when `-h`, `-p` or `-n`
   *         is the last argument.
   * @throws network::InvalidIpv4AddressException when the value of `-h` is
   *         not an IPv4 address written as four numbers: the server only
   *         listens over IPv4, and host names are not resolved.
   * @throws network::InvalidPortException when the value of `-p` is not a
   *         port from LOWEST_PORT to HIGHEST_PORT.
   * @throws InvalidNicknameOptionException when the value of `-n` is not
   *         MINIMUM_NICKNAME_SIZE to MAXIMUM_NICKNAME_SIZE bytes of UTF-8.
   */
  [[nodiscard]] static LaunchOptions read(
      std::span<const std::string_view> arguments);

  /**
   * @returns what `--help` prints: every option of the client, the logging
   *          ones included, with its default.
   */
  [[nodiscard]] static std::string usage();

  /** @returns the address and the port of the server to join. */
  [[nodiscard]] const network::Endpoint& serverEndpoint() const;

  /** @returns the nickname of the player, UTF-8. */
  [[nodiscard]] const std::string& nickname() const;

  /** @returns AudioMode::Disabled when `--no-sound` was given, otherwise
   * AudioMode::Enabled. */
  [[nodiscard]] AudioMode audioMode() const;

  /**
   * @returns whether `--help` was given: the client then prints its usage
   *          and opens no window.
   */
  [[nodiscard]] bool helpRequested() const;

 private:
  LaunchOptions(network::Endpoint serverEndpoint, std::string nickname,
                bool helpRequested);

  network::Endpoint serverEndpoint_;
  std::string nickname_;
  bool helpRequested_;
  AudioMode audioMode_{AudioMode::Enabled};
};

}  // namespace rtype::client
