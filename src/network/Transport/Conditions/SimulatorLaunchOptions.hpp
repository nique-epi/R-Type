#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "NetworkConditions.hpp"

namespace rtype::network {

/**
 * @brief The options of the network simulator, which both programs take at
 * launch and read before their own.
 */
class SimulatorLaunchOptions {
 public:
  /**
   * @brief Reads `--network <name>`, `--lag <ms>`, `--jitter <ms>`,
   * `--loss <percent>`, `--duplicates <percent>` and `--reorder <percent>`
   * from @p arguments, in order: `--network` sets the five values of its
   * scenario, every other option its own, so the last option to set a value
   * wins. Without any of them the conditions are PERFECT_NETWORK.
   *
   * Every other argument is kept for the program, see otherArguments(). An
   * argument meant as the value of an option of the program is read as one of
   * these options when it is spelled as one, as the logging options are.
   *
   * @param arguments The options keep views on these strings: they must
   * outlive the options.
   * @throws InvalidSimulatorOptionException when the value of `--network`
   * names no scenario of NETWORK_SCENARIOS, a lag or a jitter is not a whole
   * number of milliseconds up to MAXIMUM_SIMULATED_DELAY, or a share is not a
   * whole number from 0 to MAXIMUM_PERCENT.
   * @throws MissingSimulatorOptionValueException when one of these options is
   * the last argument.
   */
  [[nodiscard]] static SimulatorLaunchOptions read(
      std::span<const std::string_view> arguments);

  /** @returns The network the options describe. */
  [[nodiscard]] const NetworkConditions& conditions() const;

  /** @returns The arguments that are not options of the simulator, in the
   * order they were given. */
  [[nodiscard]] std::span<const std::string_view> otherArguments() const;

  /** @returns The part of a usage that describes these options, the
   * scenarios with their values included. */
  [[nodiscard]] static std::string usage();

 private:
  SimulatorLaunchOptions(NetworkConditions conditions,
                         std::vector<std::string_view> otherArguments);

  NetworkConditions conditions_;
  std::vector<std::string_view> otherArguments_;
};

}  // namespace rtype::network
