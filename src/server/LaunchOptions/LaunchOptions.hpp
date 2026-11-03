#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "LaunchValues.hpp"
#include "LogLaunchOptions.hpp"
#include "NetworkConditions.hpp"
#include "SimulatorLaunchOptions.hpp"

namespace rtype::server {

/**
 * @brief What the server is told at launch: the settings it is forced to, the
 * network simulated and the logging options, or a request for its usage.
 *
 * `-p`, `--seed`, `--state-rate`, `--input-delay`, `--database` and `--set`
 * do not hold a value of their own: each becomes the line of the command
 * `set` that gives the setting that value, and the lines run after
 * server.cfg, so a launch option wins over the file.
 */
class LaunchOptions {
 public:
  /**
   * @brief Reads every option of the server from @p arguments, the arguments
   * it was launched with, without its name, in one pass from the first to the
   * last: `-p <port>`, `--seed <seed>`, `--state-rate <states>`,
   * `--input-delay <ticks>`, `--database <file>`, `--set <name>=<value>`,
   * `--traffic-csv <file>` and `--help`, the options of the network simulator
   * (see network::SimulatorLaunchOptions::readOption()) and the logging options
   * (see logging::LogLaunchOptions::readOption()), whose exceptions this
   * throws too.
   *
   * The argument after an option that takes a value is always its value, even
   * when it is spelled as an option of the server, of the simulator or of the
   * logging: `--seed --log-level debug 123` refuses `--log-level` as a seed.
   * The last option on a setting wins, since the lines run in the order given,
   * and so does the last `--traffic-csv`. Without a logging option the journal
   * is SERVER_LOG_FILE_NAME next to the executable.
   *
   * @throws engine::UnknownExecutablePathException when the system does not
   *         tell where the executable is, so the default journal has no
   *         folder.
   * @throws engine::UnknownLaunchArgumentException for any other argument,
   *         `-h` included: the server takes no address.
   * @throws engine::MissingLaunchOptionValueException when an option that
   *         needs a value is the last argument.
   * @throws network::InvalidPortException when the value of `-p` is not a
   *         port from LOWEST_PORT to HIGHEST_PORT.
   * @throws engine::InvalidLaunchOptionValueException when the value of
   *         `--seed` is not a whole number from 0 to 4294967295, the value of
   *         `--state-rate` is not a whole number from
   *         LOWEST_WORLD_STATES_PER_SECOND to HIGHEST_WORLD_STATES_PER_SECOND,
   *         the value of `--input-delay` is not a whole number from
   *         LOWEST_INPUT_DELAY_TICKS to HIGHEST_INPUT_DELAY_TICKS, or the
   *         value of `--set` is not `name=value`.
   */
  [[nodiscard]] static LaunchOptions read(
      std::span<const std::string_view> arguments);

  /**
   * @returns what `--help` prints: every option of the server, the logging
   *          ones included, with its default.
   */
  [[nodiscard]] static std::string usage();

  /**
   * @returns The lines of the command `set` the launch options force, in the
   * order they were given; none when no option gave a setting.
   */
  [[nodiscard]] const std::vector<std::string>& forcedLines() const;

  /**
   * @returns whether `--help` was given: the server then prints its usage
   *          and starts nothing.
   */
  [[nodiscard]] bool helpRequested() const;

  /**
   * @returns The network the datagrams of the server cross, from the options
   * of the network simulator (see network::SimulatorLaunchOptions):
   * PERFECT_NETWORK when none was given.
   */
  [[nodiscard]] const network::NetworkConditions& networkConditions() const;

  /**
   * @returns The logging options given at launch, which the logger module
   * applies when the server starts (see logging::LogLaunchOptions::apply()).
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

}  // namespace rtype::server
