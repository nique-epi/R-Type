#pragma once

#include <string>
#include <vector>
#include "ModuleWork.hpp"

namespace rtype::engine {

/** @brief One named stage of a loop and the works added to it, in the order
 * they were added. */
struct LoopStage {
  std::string name;
  std::vector<ModuleWork> works;
};

}  // namespace rtype::engine
