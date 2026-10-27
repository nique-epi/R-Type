#include "LoopStages.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "LoopStage.hpp"
#include "StageWork.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

namespace {

void requireDistinctNames(std::span<const std::string_view> stageNames) {
  for (std::size_t position = 0; position < stageNames.size(); ++position) {
    if (std::ranges::find(stageNames.first(position), stageNames[position]) !=
        stageNames.first(position).end()) {
      throw DuplicateStageException(std::string(stageNames[position]));
    }
  }
}

std::vector<LoopStage> emptyStagesNamed(
    std::span<const std::string_view> stageNames) {
  std::vector<LoopStage> stages;
  stages.reserve(stageNames.size());
  for (const std::string_view stageName : stageNames) {
    stages.push_back(LoopStage{.name = std::string(stageName), .works = {}});
  }
  return stages;
}

void requireWork(const StageWork& work) {
  if (!work) {
    throw EmptyStageWorkException();
  }
}

}  // namespace

LoopStages::LoopStages(std::span<const std::string_view> stageNames) {
  requireDistinctNames(stageNames);
  stages_ = emptyStagesNamed(stageNames);
}

void LoopStages::add(std::string_view stageName, StageWork work) {
  requireWork(work);
  stageNamed(stageName).works.push_back(std::move(work));
}

void LoopStages::run(Duration elapsed) const {
  for (const LoopStage& stage : stages_) {
    for (const StageWork& work : stage.works) {
      work(elapsed);
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
