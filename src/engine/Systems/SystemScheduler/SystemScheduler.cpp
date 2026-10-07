#include "SystemScheduler.hpp"
#include <cstddef>
#include <memory>
#include <utility>
#include "EngineException.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

void SystemScheduler::add(std::unique_ptr<ISystem> system) {
  if (system == nullptr) {
    throw NullSystemException();
  }
  systems_.push_back(std::move(system));
}

void SystemScheduler::run(ComponentRegistry& components, Duration elapsed) {
  for (const std::unique_ptr<ISystem>& system : systems_) {
    system->update(components, elapsed);
  }
}

std::size_t SystemScheduler::size() const { return systems_.size(); }

}  // namespace rtype::engine
