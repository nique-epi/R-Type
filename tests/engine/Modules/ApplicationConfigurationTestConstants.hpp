#pragma once

/** @brief The journal line of the configuration step running. */
constexpr const char* CONFIGURE_STEP = "configure";

/** @brief The lines the configuration steps of the tests run. */
constexpr const char* TURN_FIRST_OFF = "module first off";
constexpr const char* TURN_FIRST_ON = "module first on";
constexpr const char* TURN_SECOND_OFF = "module second off";
constexpr const char* TURN_SECOND_ON = "module second on";
constexpr const char* TURN_UNKNOWN_OFF = "module unlisted off";
constexpr const char* TURN_SECOND_MAYBE = "module second maybe";

/** @brief The commands the application registers by itself. */
constexpr const char* MODULE_HELP_START = "module ";
constexpr const char* SET_HELP_START = "set ";

/** @brief What the refusals of the tests must name. */
constexpr const char* SECOND_NEEDS_FIRST =
    "module 'second' needs module 'first'";
constexpr const char* FIRST_IS_OFF = "module 'first'";
constexpr const char* KNOWN_MODULES = "first, second, third";
constexpr const char* EXPECTED_ON_OR_OFF = "expected 'on' or 'off'";
