#pragma once

#include <array>
#include <string_view>

namespace rtype::game::testing {

/** @brief A name no built-in behaviour has. */
constexpr std::string_view UNKNOWN_BEHAVIOUR_NAME = "zigzag";

/** @brief The behaviours the game is built with, in the order
 * builtInBehaviours() adds them: the names the architecture page gives. */
constexpr std::array BUILT_IN_BEHAVIOUR_NAMES{
    std::string_view{"straight"},   std::string_view{"weave"},
    std::string_view{"scatter"},    std::string_view{"reaper"},
    std::string_view{"drillback"},  std::string_view{"bydomancer"},
    std::string_view{"bydo_tyrant"}};

}  // namespace rtype::game::testing
