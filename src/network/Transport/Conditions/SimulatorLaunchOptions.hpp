#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "NetworkConditions.hpp"

namespace rtype::network {

/**
 * @brief The options of the network simulator, which both programs take at
 * launch and read among their own, one at a time.
 */
class SimulatorLaunchOptions {
 public:
  /** @brief The options of a launch that gives none: PERFECT_NETWORK. */
  SimulatorLaunchOptions();

  /**
   * @brief Reads the simulator option @p arguments starts with, if it is one:
   * `--network <name>`, `--lag <ms>`, `--jitter <ms>`, `--loss <percent>`,
   * `--duplicates <percent>` or `--reorder <percent>`.
   *
   * Its value is the argument after it, even when it looks like an option.
   * `--network` sets the five values of its scenario, every other option its
   * own, so when the options are read in the order given, the last option to
   * set a value wins.
   *
   * @returns 2, the option and its value, when @p arguments starts with one
   * of these options; 0 when it is empty or starts with another argument,
   * which reads nothing.
   * @throws InvalidSimulatorOptionException when the value of `--network`
   * names no scenario of NETWORK_SCENARIOS, a lag or a jitter is not a whole
   * number of milliseconds up to MAXIMUM_SIMULATED_DELAY, or a share is not a
   * whole number from 0 to MAXIMUM_PERCENT.
   * @throws MissingSimulatorOptionValueException when the option is the last
   * argument.
   */
  [[nodiscard]] std::size_t readOption(
      std::span<const std::string_view> arguments);

  /** @returns The network the options describe. */
  [[nodiscard]] const NetworkConditions& conditions() const;

  /** @returns The options that give a program this one starts the same
   * network: the five values, `--lag`, `--jitter`, `--loss`, `--duplicates`
   * and `--reorder`, each with its value; none for PERFECT_NETWORK. */
  [[nodiscard]] std::vector<std::string> forwardedArguments() const;

  /** @returns The part of a usage that describes these options, the
   * scenarios with their values included. */
  [[nodiscard]] static std::string usage();

 private:
  NetworkConditions conditions_;
};

}  // namespace rtype::network
