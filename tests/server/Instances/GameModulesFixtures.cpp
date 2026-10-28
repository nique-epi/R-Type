#include "GameModulesFixtures.hpp"
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include "IGameModule.hpp"
#include "RecordedModulesTestConstants.hpp"
#include "RecordingGameModule.hpp"

namespace rtype::server::testing {

std::vector<std::string> journalOf(std::initializer_list<std::string> lines) {
  return std::vector<std::string>{lines};
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
