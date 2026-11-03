#include "SimulatorLaunchOptionsFixtures.hpp"
#include <span>
#include <string_view>
#include "LaunchOptionReading.hpp"
#include "NetworkConditions.hpp"
#include "SimulatorLaunchOptions.hpp"

rtype::network::NetworkConditions conditionsOf(
    std::span<const std::string_view> arguments) {
  rtype::network::SimulatorLaunchOptions options;
  readEveryOption(options, arguments);
  return options.conditions();
}
