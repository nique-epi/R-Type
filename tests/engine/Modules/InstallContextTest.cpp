#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ApplicationTestConstants.hpp"
#include "EngineException.hpp"
#include "InstallContext.hpp"
#include "InstallContextTestConstants.hpp"
#include "LoopStages.hpp"
#include "ServiceRegistry.hpp"
#include "StageTestConstants.hpp"
#include "StageWorkFixtures.hpp"
#include "TestService.hpp"

using rtype::engine::DuplicateServiceException;
using rtype::engine::InstallContext;
using rtype::engine::LoopStages;
using rtype::engine::MissingServiceException;
using rtype::engine::ServiceRegistry;
using rtype::engine::UnknownStageException;

/**
 * Given first provided a TestService
 * When second requires a TestService
 * Then it gets the one first provided
 */
TEST(InstallContext, RequireReturnsTheServiceAModuleInstalledBeforeProvides) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  TestService service;
  InstallContext(FIRST_MODULE, services, stages).provide(service);
  const InstallContext second(SECOND_MODULE, services, stages);

  EXPECT_EQ(&second.require<TestService>(), &service);
}

/**
 * Given no module provided a TestService
 * When second requires a TestService
 * Then a MissingServiceException is thrown
 */
TEST(InstallContext, RequireOfAServiceNoModuleProvidesThrows) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  const InstallContext second(SECOND_MODULE, services, stages);

  EXPECT_THROW(static_cast<void>(second.require<TestService>()),
               MissingServiceException);
}

/**
 * Given no module provided a TestService
 * When second requires a TestService
 * Then the error names second and the TestService
 */
TEST(InstallContext, MissingServiceErrorNamesTheModuleAndTheService) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  const InstallContext second(SECOND_MODULE, services, stages);

  try {
    static_cast<void>(second.require<TestService>());
    ADD_FAILURE() << "the service was found";
  } catch (const MissingServiceException& error) {
    EXPECT_EQ(std::string{error.what()},
              SECOND_REQUIRES_ABSENT_SERVICE_MESSAGE);
  }
}

/**
 * Given first provided a TestService
 * When second looks for a TestService
 * Then it finds the one first provided
 */
TEST(InstallContext, FindReturnsTheServiceAModuleInstalledBeforeProvides) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  TestService service;
  InstallContext(FIRST_MODULE, services, stages).provide(service);
  const InstallContext second(SECOND_MODULE, services, stages);

  EXPECT_EQ(second.find<TestService>(), &service);
}

/**
 * Given no module provided a TestService
 * When second looks for a TestService
 * Then it finds none, and nothing throws
 */
TEST(InstallContext, FindOfAServiceNoModuleProvidesReturnsNull) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  const InstallContext second(SECOND_MODULE, services, stages);

  EXPECT_EQ(second.find<TestService>(), nullptr);
}

/**
 * Given first provided a TestService
 * When second provides a TestService too
 * Then a DuplicateServiceException is thrown
 */
TEST(InstallContext, SecondProviderOfAServiceThrows) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  TestService first;
  TestService second;
  InstallContext(FIRST_MODULE, services, stages).provide(first);
  InstallContext secondContext(SECOND_MODULE, services, stages);

  EXPECT_THROW(secondContext.provide(second), DuplicateServiceException);
}

/**
 * Given first provided a TestService
 * When second provides a TestService too
 * Then the error names second and the TestService
 */
TEST(InstallContext, DuplicateServiceErrorNamesTheModuleAndTheService) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  TestService first;
  TestService second;
  InstallContext(FIRST_MODULE, services, stages).provide(first);
  InstallContext secondContext(SECOND_MODULE, services, stages);

  try {
    secondContext.provide(second);
    ADD_FAILURE() << "the second service was kept";
  } catch (const DuplicateServiceException& error) {
    EXPECT_EQ(std::string{error.what()},
              SECOND_PROVIDES_PROVIDED_SERVICE_MESSAGE);
  }
}

/**
 * Given a module being installed in a loop of inputs, simulation and outputs
 * When it adds a work to inputs
 * Then the work runs when the loop runs
 */
TEST(InstallContext, AddWorkAddsTheWorkToTheNamedStage) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  std::vector<std::string> ran;
  InstallContext context(FIRST_MODULE, services, stages);
  context.addWork(INPUT_STAGE, recordingWork(ran, ADDED_WORK));

  stages.run(ELAPSED_TIME);

  EXPECT_EQ(ran, (std::vector<std::string>{ADDED_WORK}));
}

/**
 * Given a module being installed in a loop of inputs, simulation and outputs
 * When it adds a work to a stage the loop does not declare
 * Then an UnknownStageException is thrown
 */
TEST(InstallContext, AddWorkToAStageTheLoopDoesNotDeclareThrows) {
  ServiceRegistry services;
  LoopStages stages(TEST_LOOP_STAGES);
  std::vector<std::string> ran;
  InstallContext context(FIRST_MODULE, services, stages);

  EXPECT_THROW(context.addWork(UNKNOWN_STAGE, recordingWork(ran, ADDED_WORK)),
               UnknownStageException);
}
