#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "AudioMode.hpp"
#include "LaunchValues.hpp"
#include "LogLaunchOptions.hpp"
#include "NetworkConditions.hpp"
#include "PlayStart.hpp"
#include "ServerChoice.hpp"
#include "SimulatorLaunchOptions.hpp"

namespace rtype::client {

/**
 * @brief What the client is told at launch: the address and the port of the
 * server to join, the nickname of the player and the game it asks for, which
 * the connection screen starts with, whether it makes sound, the network
 * simulated and the logging options, or a request for its usage.
 *
 * `-h`, `-p`, `-n` and `--set` also force the setting they name: each gives a
 * line of the command `set`, run after client.cfg, so a launch option wins over
 * the file.
 */
class LaunchOptions {
 public:
  /**
   * @brief Reads every option of the client from @p arguments, the arguments
   * it was launched with, without its name, in one pass from the first to the
   * last: `-h <address>`, `-p <port>`, `-n <nickname>`, `--game <game>`,
   * `--set <name>=<value>`, `--no-sound`, `--traffic-csv <file>` and `--help`,
   * the options of the network simulator (see
   * network::SimulatorLaunchOptions::readOption()) and the logging options
   * (see logging::LogLaunchOptions::readOption()), whose exceptions this
   * throws too.
   *
   * The argument after an option that takes a value is always its value, even
   * when it is spelled as an option of the client, of the simulator or of the
   * logging: `-n --lag` is the nickname `--lag`. Without `-h` the address is
   * DEFAULT_SERVER_ADDRESS, without `-p` the port is DEFAULT_SERVER_PORT,
   * without `-n` the nickname is DEFAULT_NICKNAME, without `--game` the game
   * is network::ANY_GAME_CHOICE, without `--no-sound` the
   * client makes sound, and without a logging option the journal is
   * CLIENT_LOG_FILE_NAME in journalFolder(), or next to the executable when
   * that gives none. `-h` also makes PLAY join that server at once (see
   * playStart()). The last of each option wins.
   *
   * @throws engine::UnknownExecutablePathException when the default journal
   *         goes next to the executable and the system does not tell where
   *         the executable is.
   * @throws engine::UnknownLaunchArgumentException for any other argument.
   * @throws engine::MissingLaunchOptionValueException when `-h`, `-p`, `-n`
   *         or `--game` is the last argument.
   * @throws network::InvalidIpv4AddressException when the value of `-h` is
   *         not an IPv4 address written as four numbers: the server only
   *         listens over IPv4, and host names are not resolved.
   * @throws network::InvalidPortException when the value of `-p` is not a
   *         port from LOWEST_PORT to HIGHEST_PORT.
   * @throws InvalidNicknameOptionException when the value of `-n` is not
   *         MINIMUM_NICKNAME_SIZE to MAXIMUM_NICKNAME_SIZE bytes of UTF-8.
   * @throws InvalidGameOptionException when the value of `--game` is neither
   *         NEW_GAME_VALUE nor a number from network::FIRST_HOSTED_GAME_NUMBER
   *         to 65535.
   * @throws engine::InvalidLaunchOptionValueException when the value of
   *         `--set` is not `name=value`.
   */
  [[nodiscard]] static LaunchOptions read(
      std::span<const std::string_view> arguments);

  /**
   * @returns what `--help` prints: every option of the client, the logging
   *          ones included, with its default.
   */
  [[nodiscard]] static std::string usage();

  /** @returns the address and the port of the server to join, the nickname
   * of the player, UTF-8, and the game it asks for. */
  [[nodiscard]] const ServerChoice& serverChoice() const;

  /** @returns PlayStart::GivenServer when `-h` was given, so PLAY joins that
   * server at once, PlayStart::ServerMap otherwise. `--set server-address`
   * and client.cfg only fill the connection screen. */
  [[nodiscard]] PlayStart playStart() const;

  /**
   * @returns The lines of the command `set` the launch options force, in the
   * order they were given; none when no option gave a setting.
   */
  [[nodiscard]] const std::vector<std::string>& forcedLines() const;

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

  /** @returns The file given after `--traffic-csv`, nothing when it was not
   * given. */
  [[nodiscard]] const std::optional<std::filesystem::path>& trafficCsvFile()
      const;

 private:
  explicit LaunchOptions(LaunchValues values);

  LaunchValues values_;
  network::SimulatorLaunchOptions simulatorOptions_;
  logging::LogLaunchOptions logOptions_;
};

}  // namespace rtype::client
