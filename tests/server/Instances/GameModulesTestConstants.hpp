#pragma once

#include <string_view>

namespace rtype::server::testing {

constexpr std::string_view FIRST_MODULE_NAME = "first";
constexpr std::string_view SECOND_MODULE_NAME = "second";

constexpr std::string_view FIRST_INSTALLED = "first install";
constexpr std::string_view FIRST_STARTED = "first start";
constexpr std::string_view FIRST_STOPPED = "first stop";
constexpr std::string_view FIRST_IN_INPUT_STAGE = "first game inputs";
constexpr std::string_view FIRST_IN_SIMULATION_STAGE = "first game simulation";
constexpr std::string_view FIRST_IN_OUTPUT_STAGE = "first game outputs";

constexpr std::string_view SECOND_INSTALLED = "second install";
constexpr std::string_view SECOND_STARTED = "second start";
constexpr std::string_view SECOND_STOPPED = "second stop";
constexpr std::string_view SECOND_IN_INPUT_STAGE = "second game inputs";
constexpr std::string_view SECOND_IN_SIMULATION_STAGE =
    "second game simulation";
constexpr std::string_view SECOND_IN_OUTPUT_STAGE = "second game outputs";

}  // namespace rtype::server::testing
