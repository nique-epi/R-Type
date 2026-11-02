#include "SystemScheduler.hpp"
#include <cstddef>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "IClock.hpp"
#include "ISystem.hpp"
#include "SystemTime.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

void SystemScheduler::add(std::unique_ptr<ISystem> system,
                          std::string_view name) {
  if (system == nullptr) {
    throw NullSystemException();
  }
  systems_.push_back(std::move(system));
  times_.push_back(SystemTime{.name = name});
}

void SystemScheduler::run(ComponentRegistry& components, Duration elapsed) {
  if (clock_ != nullptr) {
    runMeasured(components, elapsed);
    return;
  }
  for (const std::unique_ptr<ISystem>& system : systems_) {
    system->update(components, elapsed);
  }
}

std::size_t SystemScheduler::size() const { return systems_.size(); }

void SystemScheduler::measureWith(const IClock* clock) { clock_ = clock; }

std::vector<SystemTime> SystemScheduler::takeSystemTimes() {
  std::vector<SystemTime> taken = times_;
  for (SystemTime& time : times_) {
    time.total = Duration::zero();
  }
  return taken;
}

void SystemScheduler::runMeasured(ComponentRegistry& components,
                                  Duration elapsed) {
  Duration start = clock_->now();
  for (std::size_t index = 0; index < systems_.size(); ++index) {
    systems_[index]->update(components, elapsed);
    const Duration end = clock_->now();
    times_[index].total += end - start;
    start = end;
  }
}

}  // namespace rtype::engine
