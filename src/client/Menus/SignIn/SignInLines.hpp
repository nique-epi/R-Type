#pragma once

#include <string>

namespace rtype::client {

/** @brief What the sign-in dialog shows: its title, the line under it, and
 * whether it offers a new code. */
struct SignInLines {
  std::string title;
  std::string line;
  bool offersNewCode{false};

  bool operator==(const SignInLines&) const = default;
};

}  // namespace rtype::client
