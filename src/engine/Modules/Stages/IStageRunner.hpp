#pragma once

#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief Runs the stages of a program's loop once: what the module that paces
 * the loop calls at each pass, with the time that pass stands for.
 *
 * An Application provides itself as this service before it installs its
 * modules, so the module whose thread or window paces the loop requires it
 * during its install() and calls it from then on.
 */
class IStageRunner {
 public:
  virtual ~IStageRunner() = default;

  /**
   * @brief Runs every stage in declared order, each work with @p elapsed;
   * runs nothing while the modules of the program are not all started.
   */
  virtual void runStages(Duration elapsed) const = 0;
};

}  // namespace rtype::engine
