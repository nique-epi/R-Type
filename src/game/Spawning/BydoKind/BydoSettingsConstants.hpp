#pragma once

#include <array>
#include <cstdint>

namespace rtype::game {

/**
 * @brief The values each kind of Bydo gives the settings of its behaviour,
 * until World creates every entity from its sheet: a Drifter heads left, a
 * Weaver weaves 48 logical units above and below its row every 2 s. The same
 * values stand in drifter.toml and weaver.toml.
 */
constexpr std::int64_t DRIFTER_HEADING = -1;
constexpr std::int64_t WEAVER_AMPLITUDE = 48;
constexpr std::int64_t WEAVER_PERIOD_MILLISECONDS = 2000;
constexpr std::array DRIFTER_BEHAVIOUR_SETTINGS{DRIFTER_HEADING};
constexpr std::array WEAVER_BEHAVIOUR_SETTINGS{WEAVER_AMPLITUDE,
                                               WEAVER_PERIOD_MILLISECONDS};

}  // namespace rtype::game
