#pragma once

/** @brief The error of SECOND_MODULE requiring a TestService no module
 * provides. */
constexpr const char* SECOND_REQUIRES_ABSENT_SERVICE_MESSAGE =
    "The module \"second\" requires the service TestService, which no module "
    "installed before it provides";

/** @brief The error of SECOND_MODULE providing a TestService FIRST_MODULE
 * already provides. */
constexpr const char* SECOND_PROVIDES_PROVIDED_SERVICE_MESSAGE =
    "The module \"second\" provides the service TestService, which is already "
    "provided";

/** @brief The line the work of the tests writes when it runs. */
constexpr const char* ADDED_WORK = "added work";
