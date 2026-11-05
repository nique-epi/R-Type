#include "ApplicationConfigurationFixtures.hpp"
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ApplicationConfigurationTestConstants.hpp"
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "ConfigurationStep.hpp"
#include "JournalModule.hpp"
#include "LoopStages.hpp"

rtype::engine::ConfigurationStep runningLines(
    std::vector<std::string> lines, JournalModule& journal,
    std::vector<std::string>& refusals) {
  return [lines = std::move(lines), &journal,
          &refusals](rtype::engine::CommandInterpreter& interpreter) {
    journal.calls.emplace_back(CONFIGURE_STEP);
    refusals.clear();
    for (const std::string& line : lines) {
      const rtype::engine::CommandResult result = interpreter.execute(line);
      if (!result.accepted) {
        refusals.push_back(result.message);
      }
    }
  };
}

rtype::engine::LoopStages withoutStages() {
  return rtype::engine::LoopStages(std::span<const std::string_view>{});
}
