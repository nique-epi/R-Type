#pragma once

#include <functional>
#include "InterfaceLook.hpp"

namespace rtype::client {

/** @brief Gives the look the player chose at the moment it is called, so a
 * picture follows a change made in the options on its next frame. */
using InterfaceLookSource = std::function<InterfaceLook()>;

}  // namespace rtype::client
