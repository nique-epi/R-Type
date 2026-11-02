#pragma once

#include <span>
#include <string>
#include <string_view>
#include "AudioMode.hpp"
#include "Endpoint.hpp"
#include "LogLaunchOptions.hpp"
#include "NetworkConditions.hpp"
#include "SimulatorLaunchOptions.hpp"

namespace rtype::client {

/**
 * @brief What the client is told at launch: the address and the port of the
 * server to join and the nickname of the player, which the connection screen
 * starts with, whether it makes sound, the network simulated and the logging
 * options, or a request for its usage.
 */
class LaunchOptions {
 public:
  /**
   * @brief Reads every option of the client from @p arguments, the arguments
   * it was launched with, without its name, in one pass from the first to the
   * last: `-h <address>`, `-p <port>`, `-n <nickname>`, `--no-sound` and
   * `--help`, the options of the network simulator (see
   * network::SimulatorLaunchOptions::readOption()) and the logging options
   * (see logging::LogLaunchOptions::readOption()), whose exceptions this
   * throws too.
   *
   * The argument after an option that takes a value is always its value, even
   * when it is spelled as an option of the client, of the simulator or of the
   * logging: `-n --lag` is the nickname `--lag`. Without `-h` the address is
   * DEFAULT_SERVER_ADDRESS, without `-p` the port is DEFAULT_SERVER_PORT,
   * without `-n` the nickname is DEFAULT_NICKNAME, without `--no-sound` the
   * client makes sound, and without a logging option the journal is
   * CLIENT_LOG_FILE_NAME next to the executable. The last of each option
   * wins.
   *
   * @throws engine::UnknownExecutablePathException when the system does not
   *         tell where the executable is, so the default journal has no
   *         folder.
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

  /**
   * @returns The network the datagrams of the client cross, from the options
   * of the network simulator (see network::SimulatorLaunchOptions):
   * PERFECT_NETWORK when none was given.
   */
  [[nodiscard]] const network::NetworkConditions& networkConditions() const;

  /**
   * @returns The logging options given at launch, which the logger module
   * applies when the client starts (see logging::LogLaunchOptions::apply()).
   */
  [[nodiscard]] const logging::LogLaunchOptions& logOptions() const;

 private:
  LaunchOptions(network::Endpoint serverEndpoint, std::string nickname,
                bool helpRequested);

  network::Endpoint serverEndpoint_;
  std::string nickname_;
  bool helpRequested_;
  AudioMode audioMode_{AudioMode::Enabled};
  network::SimulatorLaunchOptions simulatorOptions_;
  logging::LogLaunchOptions logOptions_;
};

}  // namespace rtype::client
