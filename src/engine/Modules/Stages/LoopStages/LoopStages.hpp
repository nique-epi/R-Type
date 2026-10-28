#pragma once

#include <span>
#include <string_view>
#include <vector>
#include "LoopStage.hpp"
#include "StageWork.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief The named stages of a program's loop, in the order the program
 * declares them, and the work the modules added to each.
 *
 * run() runs every stage in declared order and, within a stage, every work in
 * the order it was added. An Application installs its modules in list order,
 * so within a stage the works run in the order of the module list.
 */
class LoopStages {
 public:
  /**
   * @param stageNames The stages of the loop, in the order they run, for
   * example "inputs", "simulation", "outputs". It may be empty.
   */
  explicit LoopStages(std::span<const std::string_view> stageNames);

  /**
   * @brief Adds @p work at the end of the stage named @p stageName.
   * @throws UnknownStageException if the loop declares no such stage.
   */
  void add(std::string_view stageName, StageWork work);

  /** @brief Runs every stage in declared order, each work with @p elapsed. */
  void run(Duration elapsed) const;

 private:
  LoopStage& stageNamed(std::string_view stageName);

  std::vector<LoopStage> stages_;
};

}  // namespace rtype::engine
