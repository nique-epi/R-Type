#include "ApplicationFixtures.hpp"
#include <memory>
#include <vector>
#include "ApplicationTestConstants.hpp"
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "RecordingModule.hpp"
#include "RecordingModuleSetup.hpp"

using rtype::engine::IModule;

std::vector<std::unique_ptr<IModule>> recordingModules(
    const std::vector<RecordingModuleSetup>& setups, JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.reserve(setups.size());
  for (const RecordingModuleSetup& setup : setups) {
    modules.push_back(std::make_unique<RecordingModule>(setup, journal));
  }
  return modules;
}

std::vector<std::unique_ptr<IModule>> threeModules(JournalModule& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .dependencies = {}},
       RecordingModuleSetup{.name = SECOND_MODULE, .dependencies = {}},
       RecordingModuleSetup{.name = THIRD_MODULE, .dependencies = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> thirdFailingToStart(
    JournalModule& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .dependencies = {}},
       RecordingModuleSetup{.name = SECOND_MODULE, .dependencies = {}},
       RecordingModuleSetup{
           .name = THIRD_MODULE, .dependencies = {}, .failsToStart = true}},
      journal);
}

std::vector<std::unique_ptr<IModule>> firstFailingToStart(
    JournalModule& journal) {
  return recordingModules(
      {RecordingModuleSetup{
           .name = FIRST_MODULE, .dependencies = {}, .failsToStart = true},
       RecordingModuleSetup{.name = SECOND_MODULE, .dependencies = {}},
       RecordingModuleSetup{.name = THIRD_MODULE, .dependencies = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> secondDependingOnUnlisted(
    JournalModule& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .dependencies = {}},
       RecordingModuleSetup{.name = SECOND_MODULE,
                            .dependencies = {UNLISTED_MODULE}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> firstDependingOnSecond(
    JournalModule& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE,
                            .dependencies = {SECOND_MODULE}},
       RecordingModuleSetup{.name = SECOND_MODULE, .dependencies = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> secondDependingOnFirst(
    JournalModule& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .dependencies = {}},
       RecordingModuleSetup{.name = SECOND_MODULE,
                            .dependencies = {FIRST_MODULE}}},
      journal);
}
