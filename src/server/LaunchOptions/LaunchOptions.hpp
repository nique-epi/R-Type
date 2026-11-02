#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace rtype::server {

/**
 * @brief What the server is told at launch: the settings it is forced to, or a
 * request for its usage.
 *
 * `-p`, `--seed`, `--state-rate` and `--set` do not hold a value of their own:
 * each becomes the line of the command `set` that gives the setting that value,
 * and the lines run after server.cfg, so a launch option wins over the file.
 */
class LaunchOptions {
 public:
  /**
   * @brief Reads `-p <port>`, `--seed <seed>`, `--state-rate <states>`,
   * `--set <name>=<value>` and `--help` from @p arguments, the arguments the
   * logging options left (see logging::LogLaunchOptions).
   *
   * The last option on a setting wins, since the lines run in the order given.
   *
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
   *         or the value of `--set` is not `name=value` with a value holding
   *         no quote.
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

 private:
  std::vector<std::string> forcedLines_;
  bool helpRequested_{false};
};

}  // namespace rtype::server
