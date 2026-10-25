#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/** @brief What a quit confirmation is built from, but the picture it owns. */
struct QuitConfirmationScreenParts {
  /** @brief Screens the dialog lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
};

}  // namespace rtype::client
