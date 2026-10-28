#pragma once

/** @brief The error of SECOND_MODULE requiring a TestService no module
 * provides is these two parts around the name of the TestService. */
constexpr const char* SECOND_REQUIRES_MESSAGE_BEFORE_SERVICE =
    "The module \"second\" requires the service ";
constexpr const char* SECOND_REQUIRES_MESSAGE_AFTER_SERVICE =
    ", which no module installed before it provides";

/** @brief The error of SECOND_MODULE providing a TestService FIRST_MODULE
 * already provides is these two parts around the name of the TestService. */
constexpr const char* SECOND_PROVIDES_MESSAGE_BEFORE_SERVICE =
    "The module \"second\" provides the service ";
constexpr const char* SECOND_PROVIDES_MESSAGE_AFTER_SERVICE =
    ", which is already provided";

/** @brief The line the work of the tests writes when it runs. */
constexpr const char* ADDED_WORK = "added work";
