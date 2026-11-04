#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "LogLaunchOptions.hpp"
#include "MasterLaunchValues.hpp"

namespace rtype::master {

/**
 * @brief What the master is told at launch: the settings it is forced to and
 * the logging options, or a request for its usage.
 *
 * `-p`, `--database` and `--set` do not hold a value of their own: each
 * becomes the line of the command `set` that gives the setting that value, and
 * the lines run after master.cfg, so a launch option wins over the file.
 */
class MasterLaunchOptions {
 public:
  /**
   * @brief Reads every option of the master from @p arguments, the arguments
   * it was launched with after `--mode master`, in one pass: `-p <port>`,
   * `--database <file>`, `--set <name>=<value>` and `--help`, and the logging
   * options (see logging::LogLaunchOptions::readOption()), whose exceptions
   * this throws too. Without a logging option the journal is
   * MASTER_LOG_FILE_NAME next to the executable.
   *
   * @throws engine::UnknownExecutablePathException when the system does not
   *         tell where the executable is, so the default journal has no
   *         folder.
   * @throws engine::UnknownLaunchArgumentException for any other argument.
   * @throws engine::MissingLaunchOptionValueException when an option that
   *         needs a value is the last argument.
   * @throws network::InvalidPortException when the value of `-p` is not a
   *         port from LOWEST_PORT to HIGHEST_PORT.
   * @throws engine::InvalidLaunchOptionValueException when the value of
   *         `--set` is not `name=value`.
   */
  [[nodiscard]] static MasterLaunchOptions read(
      std::span<const std::string_view> arguments);

  /**
   * @returns what `--help` prints: every option of the master, the logging
   *          ones included, with its default.
   */
  [[nodiscard]] static std::string usage();

  /**
   * @returns The lines of the command `set` the launch options force, in the
   * order they were given; none when no option gave a setting.
   */
  [[nodiscard]] const std::vector<std::string>& forcedLines() const;

  /**
   * @returns whether `--help` was given: the master then prints its usage
   *          and starts nothing.
   */
  [[nodiscard]] bool helpRequested() const;

  /**
   * @returns The logging options given at launch, which the logger module
   * applies when the master starts (see logging::LogLaunchOptions::apply()).
   */
  [[nodiscard]] const logging::LogLaunchOptions& logOptions() const;

 private:
  explicit MasterLaunchOptions(MasterLaunchValues values);

  MasterLaunchValues values_;
  logging::LogLaunchOptions logOptions_;
};

}  // namespace rtype::master
