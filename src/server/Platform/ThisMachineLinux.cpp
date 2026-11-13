#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include "MachineCounters.hpp"
#include "MachineCountersConstants.hpp"
#include "ProcCounters.hpp"
#include "ProcTexts.hpp"
#include "ThisMachine.hpp"

namespace rtype::server {

namespace {

/** @returns The text of @p file, empty when it cannot be read. */
std::string textOf(std::string_view file) {
  std::ifstream stream{std::string{file}};
  return std::string{std::istreambuf_iterator<char>{stream},
                     std::istreambuf_iterator<char>{}};
}

}  // namespace

std::optional<MachineCounters> countersOfThisMachine() {
  const std::string processors = textOf(PROCESSORS_FILE);
  const std::string memory = textOf(MEMORY_FILE);
  return countersIn(ProcTexts{.processors = processors, .memory = memory});
}

}  // namespace rtype::server
