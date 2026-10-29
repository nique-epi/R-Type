#pragma once

#include <string>
#include <vector>
#include "StageWork.hpp"

namespace rtype::engine {

/** @brief One named stage of a loop and the works added to it, in the order
 * they were added. */
struct LoopStage {
  std::string name;
  std::vector<StageWork> works;
};

}  // namespace rtype::engine
