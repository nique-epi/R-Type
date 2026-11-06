#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "GameLaunchValues.hpp"
#include "LogLaunchOptions.hpp"
#include "NetworkConditions.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "SupervisorLink.hpp"

namespace rtype::server {

/**
 * @brief What a game process is told at launch by the supervisor that starts
 * it: its port, its database, the supervisor it reports to, the network
 * simulated and the logging options, or a request for its usage.
 *
 * `-p` and `--database` become lines of the command `set`, which run after
 * server.cfg, so a launch option wins over the file.
 */
class GameLaunchOptions {
 public:
  /**
   * @brief Reads every option of a game process from @p arguments, the
   * arguments after `--mode game`, in one pass: `-p <port>`,
   * `--supervisor <port>`, `--link-token <number>`, `--database <file>`,
   * `--traffic-csv <file>` and `--help`, the options of the network simulator
   * (see network::SimulatorLaunchOptions::readOption()) and the logging
   * options (see logging::LogLaunchOptions::readOption()), whose exceptions
   * this throws too. Without a logging option the journal is
   * SERVER_LOG_FILE_NAME next to the executable.
   *
   * @throws engine::UnknownExecutablePathException when the system does not
   *         tell where the executable is, so the default journal has no
   *         folder.
   * @throws engine::UnknownLaunchArgumentException for any other argument.
   * @throws engine::MissingLaunchOptionValueException when an option that
   *         needs a value is the last argument.
   * @throws network::InvalidPortException when the value of `-p` or of
   *         `--supervisor` is not a port from LOWEST_PORT to HIGHEST_PORT.
   * @throws engine::InvalidLaunchOptionValueException when the value of
   *         `--link-token` is 0 or not a whole number.
   * @throws MissingSupervisorLinkException when `--help` was not given and
   *         `--supervisor` or `--link-token` was not.
   */
  [[nodiscard]] static GameLaunchOptions read(
      std::span<const std::string_view> arguments);

  /** @returns what `--help` prints: every option of a game process, the
   * logging ones included, with its default. */
  [[nodiscard]] static std::string usage();

  /**
   * @returns The lines of the command `set` the launch options force, in the
   * order they were given.
   */
  [[nodiscard]] const std::vector<std::string>& forcedLines() const;

  /**
   * @returns The port of the supervisor and the token shared with it. read()
   * refuses a launch without them unless `--help` was given, when this is an
   * empty link.
   */
  [[nodiscard]] SupervisorLink supervisorLink() const;

  /** @returns whether `--help` was given: the game then prints its usage and
   * starts nothing. */
  [[nodiscard]] bool helpRequested() const;

  /**
   * @returns The network the datagrams of the game cross, from the options of
   * the network simulator: PERFECT_NETWORK when none was given.
   */
  [[nodiscard]] const network::NetworkConditions& networkConditions() const;

  /** @returns The logging options given at launch, which the logger module
   * applies when the game starts. */
  [[nodiscard]] const logging::LogLaunchOptions& logOptions() const;

  /** @returns The file given after `--traffic-csv`, nothing when it was not
   * given. */
  [[nodiscard]] const std::optional<std::filesystem::path>& trafficCsvFile()
      const;

 private:
  explicit GameLaunchOptions(GameLaunchValues values);

  GameLaunchValues values_;
  network::SimulatorLaunchOptions simulatorOptions_;
  logging::LogLaunchOptions logOptions_;
};

}  // namespace rtype::server
