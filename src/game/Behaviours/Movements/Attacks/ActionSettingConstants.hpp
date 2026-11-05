#pragma once

#include <string_view>
#include "WholeNumberRange.hpp"

namespace rtype::game {

/**
 * @brief The settings every behaviour that attacks names the same way, in
 * milliseconds: how long it rests between two attacks, and how long the
 * signal, the strike and the recover of an attack last (see ActionTimings).
 * A signal or a strike lasts at least 1 ms; a recover of 0 is skipped.
 */
constexpr std::string_view REST_SETTING_NAME = "rest_ms";
constexpr std::string_view SIGNAL_SETTING_NAME = "signal_ms";
constexpr std::string_view STRIKE_SETTING_NAME = "strike_ms";
constexpr std::string_view RECOVER_SETTING_NAME = "recover_ms";
constexpr engine::WholeNumberRange REST_RANGE{.lowest = 0, .highest = 60000};
constexpr engine::WholeNumberRange PHASE_RANGE{.lowest = 1, .highest = 60000};
constexpr engine::WholeNumberRange RECOVER_RANGE{.lowest = 0, .highest = 60000};

}  // namespace rtype::game
