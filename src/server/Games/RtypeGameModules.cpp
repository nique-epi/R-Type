#include "RtypeGameModules.hpp"
#include <memory>
#include <vector>
#include "IGameModule.hpp"
#include "ReplicationModule.hpp"
#include "RtypeRulesModule.hpp"

namespace rtype::server {

std::vector<std::unique_ptr<IGameModule>> rtypeGameModules() {
  std::vector<std::unique_ptr<IGameModule>> modules;
  modules.push_back(std::make_unique<ReplicationModule>());
  modules.push_back(std::make_unique<RtypeRulesModule>());
  return modules;
}

}  // namespace rtype::server
