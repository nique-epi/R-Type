#pragma once

#include <string>
#include "StageWork.hpp"

namespace rtype::engine {

/** @brief A work added to a stage and the name of the module that added it,
 * empty for a work no module owns. */
struct ModuleWork {
  std::string module;
  StageWork work;
};

}  // namespace rtype::engine
