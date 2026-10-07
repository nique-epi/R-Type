#pragma once

#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

/**
 * @brief One behavior of the game, applied to the components every step.
 */
class ISystem {
 public:
  virtual ~ISystem() = default;

  /**
   * @param components Registry holding the components to read and update.
   * @param elapsed Simulation time covered by this step.
   */
  virtual void update(ComponentRegistry& components, Duration elapsed) = 0;
};

}  // namespace rtype::engine
