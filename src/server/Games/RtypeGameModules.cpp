#include "RtypeGameModules.hpp"
#include <memory>
#include <vector>
#include "ChatModule.hpp"
#include "IGameModule.hpp"
#include "IResultsArchive.hpp"
#include "ReplicationModule.hpp"
#include "ResultsModule.hpp"
#include "RtypeRulesModule.hpp"

namespace rtype::server {

std::vector<std::unique_ptr<IGameModule>> rtypeGameModules(
    IResultsArchive& results) {
  std::vector<std::unique_ptr<IGameModule>> modules;
  modules.push_back(std::make_unique<ReplicationModule>());
  modules.push_back(std::make_unique<RtypeRulesModule>());
  modules.push_back(std::make_unique<ResultsModule>(results));
  modules.push_back(std::make_unique<ChatModule>());
  return modules;
}

}  // namespace rtype::server
