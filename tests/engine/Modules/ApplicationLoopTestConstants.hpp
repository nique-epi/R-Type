#pragma once

#include <chrono>

/** @brief The names of the modules of the tests that provide the services and
 * that subscribe to the bus. */
constexpr const char* PROVIDER_MODULE = "provider";
constexpr const char* SUBSCRIBER_MODULE = "subscriber";
constexpr const char* APPLICATION_FINDING_MODULE = "application finding";
constexpr const char* SLOW_STAGE_MODULE = "slow stage";

/** @brief The journal lines of the slow stage work and of stop(). */
constexpr const char* SLOW_WORK_ENDED = "slow work ended";
constexpr const char* STOP_RETURNED = "stop returned";

/** @brief How long the slow work keeps running once stop() was called, long
 * enough for a stop() that does not wait for it to return first. */
constexpr std::chrono::milliseconds SLOW_WORK_DURATION{100};

/** @brief The journal lines of the works of StageModules, a stage then a
 * module. */
constexpr const char* INPUTS_OF_SECOND = "inputs second";
constexpr const char* SIMULATION_OF_FIRST = "simulation first";
constexpr const char* SIMULATION_OF_SECOND = "simulation second";
constexpr const char* SIMULATION_OF_THIRD = "simulation third";
constexpr const char* OUTPUTS_OF_FIRST = "outputs first";

/** @brief The journal lines of a ServiceUsingModule named SECOND_MODULE. */
constexpr const char* SERVICE_FOUND_BY_SECOND = "service found by second";
constexpr const char* SERVICE_ABSENT_FOR_SECOND = "service absent for second";
