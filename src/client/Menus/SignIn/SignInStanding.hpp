#pragma once

#include <chrono>
#include "Language.hpp"

namespace rtype::client {

/** @brief What the lines of the sign-in dialog depend on besides the link:
 * the time now, whether the master refused the session, and the language the
 * lines are written in. */
struct SignInStanding {
  std::chrono::sys_seconds now;
  bool sessionRefused = false;
  Language language = Language::English;
};

}  // namespace rtype::client
