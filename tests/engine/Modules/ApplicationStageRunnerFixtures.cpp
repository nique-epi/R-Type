#include "ApplicationStageRunnerFixtures.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "ApplicationTestConstants.hpp"
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "PacingModule.hpp"
#include "StageModule.hpp"
#include "StageModuleSetup.hpp"
#include "StageTestConstants.hpp"

using rtype::engine::IModule;

std::vector<std::unique_ptr<IModule>> pacingThenSecondInSimulation(
    std::unique_ptr<PacingModule> pacing, JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(std::move(pacing));
  modules.push_back(std::make_unique<StageModule>(
      StageModuleSetup{.name = SECOND_MODULE, .stage = SIMULATION_STAGE},
      journal));
  return modules;
}
