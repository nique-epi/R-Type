#include "GameModulesFixtures.hpp"
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "GameModulesTestConstants.hpp"
#include "IGameModule.hpp"
#include "RecordingGameModule.hpp"

namespace rtype::server::testing {

std::vector<std::string> journalOf(
    std::initializer_list<std::string_view> lines) {
  std::vector<std::string> journal;
  for (const std::string_view line : lines) {
    journal.emplace_back(line);
  }
  return journal;
}

std::vector<std::unique_ptr<IGameModule>> twoRecordingModules(
    std::vector<std::string>& journal, bool secondFailsToStart) {
  std::vector<std::unique_ptr<IGameModule>> modules;
  modules.push_back(std::make_unique<RecordingGameModule>(
      std::string{FIRST_MODULE_NAME}, journal, false));
  modules.push_back(std::make_unique<RecordingGameModule>(
      std::string{SECOND_MODULE_NAME}, journal, secondFailsToStart));
  return modules;
}

}  // namespace rtype::server::testing
