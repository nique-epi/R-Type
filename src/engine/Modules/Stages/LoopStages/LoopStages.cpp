#include "LoopStages.hpp"
#include <algorithm>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "LoopStage.hpp"
#include "ModuleWork.hpp"
#include "StageWork.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

namespace {

std::vector<LoopStage> emptyStagesNamed(
    std::span<const std::string_view> stageNames) {
  std::vector<LoopStage> stages;
  stages.reserve(stageNames.size());
  for (const std::string_view stageName : stageNames) {
    stages.push_back(LoopStage{.name = std::string(stageName), .works = {}});
  }
  return stages;
}

}  // namespace

LoopStages::LoopStages(std::span<const std::string_view> stageNames)
    : stages_(emptyStagesNamed(stageNames)) {}

void LoopStages::add(std::string_view stageName, StageWork work) {
  add(stageName, std::string_view{}, std::move(work));
}

void LoopStages::add(std::string_view stageName, std::string_view moduleName,
                     StageWork work) {
  stageNamed(stageName).works.push_back(
      ModuleWork{.module = std::string(moduleName), .work = std::move(work)});
}

void LoopStages::removeWorkOf(std::string_view moduleName) {
  for (LoopStage& stage : stages_) {
    std::erase_if(stage.works, [moduleName](const ModuleWork& entry) {
      return entry.module == moduleName;
    });
  }
}

void LoopStages::run(Duration elapsed) const {
  for (const LoopStage& stage : stages_) {
    for (const ModuleWork& entry : stage.works) {
      entry.work(elapsed);
    }
  }
}

LoopStage& LoopStages::stageNamed(std::string_view stageName) {
  const auto found = std::ranges::find(
      stages_, stageName,
      [](const LoopStage& stage) { return std::string_view(stage.name); });
  if (found == stages_.end()) {
    throw UnknownStageException(std::string(stageName));
  }
  return *found;
}

}  // namespace rtype::engine
