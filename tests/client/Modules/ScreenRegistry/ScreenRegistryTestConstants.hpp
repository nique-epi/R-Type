#pragma once

#include "ScreenId.hpp"

/** @brief Screen the tests register as a module that was added after the
 * others would. */
constexpr rtype::client::ScreenId EXTRA_SCREEN{"an extra screen"};

/** @brief A second screen, to tell two registrations apart. */
constexpr rtype::client::ScreenId OTHER_SCREEN{"another screen"};

/** @brief The context the tests build screens from: the registry does not look
 * at it, it only hands it to the builder. */
constexpr int CONTEXT_OF_THE_TEST = 7;
