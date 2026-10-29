#pragma once

#include "BasicScreenRegistry.hpp"
#include "ScreenContext.hpp"

namespace rtype::client {

/** @brief The registry of the screens of the client, whose builders are given
 * the ScreenContext. */
using ScreenRegistry = BasicScreenRegistry<ScreenContext>;

}  // namespace rtype::client
