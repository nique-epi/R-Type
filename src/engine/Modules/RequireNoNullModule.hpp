#pragma once

#include <memory>
#include <span>
#include "EngineException.hpp"

namespace rtype::engine {

/**
 * @brief Checks that a list of modules, of an application or of a game, holds
 * no null pointer.
 * @throws NullModuleException when it does.
 */
template <typename Module>
void requireNoNullModule(std::span<const std::unique_ptr<Module>> modules) {
  for (const std::unique_ptr<Module>& listedModule : modules) {
    if (listedModule == nullptr) {
      throw NullModuleException();
    }
  }
}

}  // namespace rtype::engine
