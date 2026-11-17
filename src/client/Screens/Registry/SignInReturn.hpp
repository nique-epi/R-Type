#pragma once

#include <optional>
#include "ScreenId.hpp"

namespace rtype::client {

/**
 * @brief Where SIGN_IN_SCREEN leads once the player signed in: it closes
 * itself, then the dialog that asked when closesAsking, then opens screen
 * when it names one, the screen that dialog stood in for. Asked from the
 * title, it only closes itself.
 */
struct SignInReturn {
  bool closesAsking{false};
  std::optional<ScreenId> screen;
};

}  // namespace rtype::client
