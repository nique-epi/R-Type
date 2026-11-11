#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The panel that downloads, installs and restarts, over the title. */
constexpr ScreenId UPDATE_SCREEN{"update"};

/** @brief The notes of the release just installed, over the title. */
constexpr ScreenId WHATS_NEW_SCREEN{"whats-new"};

}  // namespace rtype::client
