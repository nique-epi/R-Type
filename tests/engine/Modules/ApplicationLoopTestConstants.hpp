#pragma once

/** @brief The names of the modules of the tests that provide the services and
 * that subscribe to the bus. */
constexpr const char* PROVIDER_MODULE = "provider";
constexpr const char* SUBSCRIBER_MODULE = "subscriber";

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
