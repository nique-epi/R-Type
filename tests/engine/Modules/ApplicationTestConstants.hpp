#pragma once

/** @brief The names of the modules of the tests, listed in this order. */
constexpr const char* FIRST_MODULE = "first";
constexpr const char* SECOND_MODULE = "second";
constexpr const char* THIRD_MODULE = "third";

/** @brief A name no module of the tests has. */
constexpr const char* UNLISTED_MODULE = "unlisted";

/** @brief The journal lines of the calls the modules of the tests receive. */
constexpr const char* INSTALL_FIRST = "install first";
constexpr const char* INSTALL_SECOND = "install second";
constexpr const char* INSTALL_THIRD = "install third";
constexpr const char* START_FIRST = "start first";
constexpr const char* START_SECOND = "start second";
constexpr const char* START_THIRD = "start third";
constexpr const char* STOP_FIRST = "stop first";
constexpr const char* STOP_SECOND = "stop second";
constexpr const char* STOP_THIRD = "stop third";

/** @brief The error of SECOND_MODULE requiring UNLISTED_MODULE. */
constexpr const char* SECOND_REQUIRES_UNLISTED_MESSAGE =
    "The module \"second\" requires the module \"unlisted\", which is not "
    "listed before it";

/** @brief The error of THIRD_MODULE throwing START_REFUSAL_REASON from start().
 */
constexpr const char* THIRD_FAILED_TO_START_MESSAGE =
    "The module \"third\" failed to start: the test refuses this start";
