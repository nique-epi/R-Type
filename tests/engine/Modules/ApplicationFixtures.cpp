#include "ApplicationFixtures.hpp"
#include <memory>
#include <vector>
#include "ApplicationTestConstants.hpp"
#include "IModule.hpp"
#include "ModuleJournal.hpp"
#include "RecordingModule.hpp"
#include "RecordingModuleSetup.hpp"

using rtype::engine::IModule;

std::vector<std::unique_ptr<IModule>> recordingModules(
    const std::vector<RecordingModuleSetup>& setups, ModuleJournal& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.reserve(setups.size());
  for (const RecordingModuleSetup& setup : setups) {
    modules.push_back(std::make_unique<RecordingModule>(setup, journal));
  }
  return modules;
}

std::vector<std::unique_ptr<IModule>> threeModules(ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = THIRD_MODULE, .requiredModules = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> thirdFailingToStart(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE, .requiredModules = {}},
       RecordingModuleSetup{
           .name = THIRD_MODULE, .requiredModules = {}, .failsToStart = true}},
      journal);
}

std::vector<std::unique_ptr<IModule>> firstFailingToStart(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{
           .name = FIRST_MODULE, .requiredModules = {}, .failsToStart = true},
       RecordingModuleSetup{.name = SECOND_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = THIRD_MODULE, .requiredModules = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> secondRequiringUnlisted(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE,
                            .requiredModules = {UNLISTED_MODULE}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> firstRequiringSecond(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE,
                            .requiredModules = {SECOND_MODULE}},
       RecordingModuleSetup{.name = SECOND_MODULE, .requiredModules = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> secondRequiringFirst(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE,
                            .requiredModules = {FIRST_MODULE}}},
      journal);
}
