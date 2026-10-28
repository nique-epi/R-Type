#pragma once

#include <memory>
#include <string>
#include <vector>
#include "DeliveredEvents.hpp"
#include "EventBus.hpp"
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "TestService.hpp"

/** @returns FIRST_MODULE adding its work to OUTPUT_STAGE, SECOND_MODULE to
 * INPUT_STAGE and THIRD_MODULE to SIMULATION_STAGE. */
std::vector<std::unique_ptr<rtype::engine::IModule>>
firstInOutputsSecondInInputsThirdInSimulation(JournalModule& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, each adding its work
 * to SIMULATION_STAGE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> threeModulesInSimulation(
    JournalModule& journal);

/** @returns A RecordingModule FIRST_MODULE, SECOND_MODULE requiring the
 * TestService no module provides, then a RecordingModule THIRD_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>>
secondRequiringAnAbsentService(JournalModule& journal);

/** @returns A RecordingModule FIRST_MODULE, then SECOND_MODULE looking for the
 * TestService no module provides. */
std::vector<std::unique_ptr<rtype::engine::IModule>>
secondLookingForAnAbsentService(JournalModule& journal);

/** @returns PROVIDER_MODULE providing @p service and @p bus, then
 * SECOND_MODULE requiring the TestService. */
std::vector<std::unique_ptr<rtype::engine::IModule>>
providerThenSecondRequiringTheService(TestService& service,
                                      rtype::engine::EventBus& bus,
                                      JournalModule& journal);

/** @returns SECOND_MODULE looking for the TestService, then PROVIDER_MODULE
 * providing @p service and @p bus. */
std::vector<std::unique_ptr<rtype::engine::IModule>>
secondLookingBeforeTheProvider(TestService& service,
                               rtype::engine::EventBus& bus,
                               JournalModule& journal);

/** @returns PROVIDER_MODULE providing @p service and @p bus, then
 * SUBSCRIBER_MODULE counting in @p delivered the TestEvents of @p bus. */
std::vector<std::unique_ptr<rtype::engine::IModule>> providerThenSubscriber(
    TestService& service, rtype::engine::EventBus& bus,
    DeliveredEvents& delivered);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, each adding its work
 * to SIMULATION_STAGE; THIRD_MODULE throws from start(). */
std::vector<std::unique_ptr<rtype::engine::IModule>>
threeModulesInSimulationThirdFailingToStart(JournalModule& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, each appending its
 * name to @p destroyed when it is destroyed. */
std::vector<std::unique_ptr<rtype::engine::IModule>>
threeModulesRecordingTheirDestruction(std::vector<std::string>& destroyed);
