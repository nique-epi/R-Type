#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief The screen turning off as the design draws it: the whole picture at
 * first, a line once the picture is 3 rows or fewer, dark from 350 ms. */
constexpr int WHOLE_PICTURE_ROWS = 270;
constexpr int WHOLE_LINE_HALF_WIDTH = 240;
constexpr rtype::engine::Duration COLLAPSE_END = std::chrono::milliseconds(200);
constexpr rtype::engine::Duration JUST_BEFORE_DARK =
    std::chrono::milliseconds(349);
constexpr rtype::engine::Duration DARK = std::chrono::milliseconds(350);
