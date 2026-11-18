#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The dialog that signs the game in through the web site, pushed over
 * the screen that asked for it: it shows the short code to validate, opens the
 * page in the browser, and once signed in, opens SignInReturn::screen again in
 * place of the dialog that asked. */
constexpr ScreenId SIGN_IN_SCREEN{"sign in"};

}  // namespace rtype::client
