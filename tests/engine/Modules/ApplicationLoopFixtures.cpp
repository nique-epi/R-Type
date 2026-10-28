#include "ApplicationLoopFixtures.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "ApplicationLoopTestConstants.hpp"
#include "ApplicationTestConstants.hpp"
#include "DeliveredEvents.hpp"
#include "DestructionRecordingModule.hpp"
#include "EventBus.hpp"
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "ProvidingModule.hpp"
#include "RecordingModule.hpp"
#include "RecordingModuleSetup.hpp"
#include "ServiceUsingModule.hpp"
#include "ServiceUsingModuleSetup.hpp"
#include "StageModule.hpp"
#include "StageModuleSetup.hpp"
#include "StageTestConstants.hpp"
#include "SubscribingModule.hpp"
#include "TestService.hpp"

using rtype::engine::EventBus;
using rtype::engine::IModule;

std::vector<std::unique_ptr<IModule>>
firstInOutputsSecondInInputsThirdInSimulation(JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(std::make_unique<StageModule>(
      StageModuleSetup{.name = FIRST_MODULE, .stage = OUTPUT_STAGE}, journal));
  modules.push_back(std::make_unique<StageModule>(
      StageModuleSetup{.name = SECOND_MODULE, .stage = INPUT_STAGE}, journal));
  modules.push_back(std::make_unique<StageModule>(
      StageModuleSetup{.name = THIRD_MODULE, .stage = SIMULATION_STAGE},
      journal));
  return modules;
}

std::vector<std::unique_ptr<IModule>> threeModulesInSimulation(
    JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  for (const char* name : {FIRST_MODULE, SECOND_MODULE, THIRD_MODULE}) {
    modules.push_back(std::make_unique<StageModule>(
        StageModuleSetup{.name = name, .stage = SIMULATION_STAGE}, journal));
  }
  return modules;
}

std::vector<std::unique_ptr<IModule>> secondRequiringAnAbsentService(
    JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(std::make_unique<RecordingModule>(
      RecordingModuleSetup{.name = FIRST_MODULE, .dependencies = {}}, journal));
  modules.push_back(std::make_unique<ServiceUsingModule>(
      ServiceUsingModuleSetup{.name = SECOND_MODULE, .requiresService = true},
      journal));
  modules.push_back(std::make_unique<RecordingModule>(
      RecordingModuleSetup{.name = THIRD_MODULE, .dependencies = {}}, journal));
  return modules;
}

std::vector<std::unique_ptr<IModule>> secondLookingForAnAbsentService(
    JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(std::make_unique<RecordingModule>(
      RecordingModuleSetup{.name = FIRST_MODULE, .dependencies = {}}, journal));
  modules.push_back(std::make_unique<ServiceUsingModule>(
      ServiceUsingModuleSetup{.name = SECOND_MODULE, .requiresService = false},
      journal));
  return modules;
}

std::vector<std::unique_ptr<IModule>> providerThenSecondRequiringTheService(
    TestService& service, EventBus& bus, JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(
      std::make_unique<ProvidingModule>(PROVIDER_MODULE, service, bus));
  modules.push_back(std::make_unique<ServiceUsingModule>(
      ServiceUsingModuleSetup{.name = SECOND_MODULE, .requiresService = true},
      journal));
  return modules;
}

std::vector<std::unique_ptr<IModule>> secondLookingBeforeTheProvider(
    TestService& service, EventBus& bus, JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(std::make_unique<ServiceUsingModule>(
      ServiceUsingModuleSetup{.name = SECOND_MODULE, .requiresService = false},
      journal));
  modules.push_back(
      std::make_unique<ProvidingModule>(PROVIDER_MODULE, service, bus));
  return modules;
}

std::vector<std::unique_ptr<IModule>> providerThenSubscriber(
    TestService& service, EventBus& bus, DeliveredEvents& delivered) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(
      std::make_unique<ProvidingModule>(PROVIDER_MODULE, service, bus));
  modules.push_back(
      std::make_unique<SubscribingModule>(SUBSCRIBER_MODULE, delivered));
  return modules;
}

std::vector<std::unique_ptr<IModule>>
threeModulesInSimulationThirdFailingToStart(JournalModule& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  for (const char* name : {FIRST_MODULE, SECOND_MODULE, THIRD_MODULE}) {
    modules.push_back(std::make_unique<StageModule>(
        StageModuleSetup{
            .name = name,
            .stage = SIMULATION_STAGE,
            .failsToStart = std::string_view{name} == THIRD_MODULE},
        journal));
  }
  return modules;
}

std::vector<std::unique_ptr<IModule>> threeModulesRecordingTheirDestruction(
    std::vector<std::string>& destroyed) {
  std::vector<std::unique_ptr<IModule>> modules;
  for (const char* name : {FIRST_MODULE, SECOND_MODULE, THIRD_MODULE}) {
    modules.push_back(
        std::make_unique<DestructionRecordingModule>(name, destroyed));
  }
  return modules;
}
