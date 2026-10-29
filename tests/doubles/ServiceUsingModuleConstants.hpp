#pragma once

/** @brief What a ServiceUsingModule writes in its journal before its name,
 * once its install() found the TestService or found it absent. */
constexpr const char* SERVICE_FOUND_CALL = "service found by ";
constexpr const char* SERVICE_ABSENT_CALL = "service absent for ";
