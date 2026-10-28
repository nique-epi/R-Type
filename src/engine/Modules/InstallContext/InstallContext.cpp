#include "InstallContext.hpp"
#include <string>
#include <string_view>
#include <typeindex>
#include <utility>
#include "EngineException.hpp"
#include "LoopStages.hpp"
#include "ServiceRegistry.hpp"
#include "StageWork.hpp"

namespace rtype::engine {

InstallContext::InstallContext(std::string moduleName,
                               ServiceRegistry& services, LoopStages& stages)
    : moduleName_(std::move(moduleName)),
      services_(&services),
      stages_(&stages) {}

void InstallContext::addWork(std::string_view stageName, StageWork work) {
  stages_->add(stageName, std::move(work));
}

void InstallContext::refuseSecondProvider(std::type_index service) const {
  throw DuplicateServiceException(moduleName_, service.name());
}

void InstallContext::refuseMissingService(std::type_index service) const {
  throw MissingServiceException(moduleName_, service.name());
}

}  // namespace rtype::engine
