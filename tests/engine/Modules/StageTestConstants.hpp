#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief The stages of the loop of the tests, declared in this order. */
constexpr const char* INPUT_STAGE = "inputs";
constexpr const char* SIMULATION_STAGE = "simulation";
constexpr const char* OUTPUT_STAGE = "outputs";
constexpr std::array<std::string_view, 3> TEST_LOOP_STAGES{
    INPUT_STAGE, SIMULATION_STAGE, OUTPUT_STAGE};

/** @brief A stage the loop of the tests does not declare. */
constexpr const char* UNKNOWN_STAGE = "rendering";

/** @brief The time one pass of the loop of the tests stands for. */
constexpr rtype::engine::Duration ELAPSED_TIME = std::chrono::milliseconds(16);
