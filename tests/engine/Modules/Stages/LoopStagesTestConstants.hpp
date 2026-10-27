#pragma once

#include <array>
#include <string_view>
#include "StageTestConstants.hpp"

/** @brief A loop that declares INPUT_STAGE twice. */
constexpr std::array<std::string_view, 3> INPUTS_DECLARED_TWICE{
    INPUT_STAGE, OUTPUT_STAGE, INPUT_STAGE};

/** @brief The lines the works of the tests write when they run. */
constexpr const char* FIRST_WORK = "first work";
constexpr const char* SECOND_WORK = "second work";
constexpr const char* THIRD_WORK = "third work";

/** @brief The error of a work added to UNKNOWN_STAGE. */
constexpr const char* UNKNOWN_STAGE_MESSAGE =
    "The loop has no stage named \"rendering\"";
