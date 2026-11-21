#pragma once

#include <optional>
#include <string>

namespace rtype::client {

/** @brief What the sign-in dialog shows: its title, the line under it, how
 * long the code stays valid while one waits, and whether it offers a new
 * code. A dialog with a validity shows its title as the code, large. */
struct SignInLines {
  std::string title;
  std::string line;
  std::optional<std::string> validity;
  bool offersNewCode{false};

  bool operator==(const SignInLines&) const = default;
};

}  // namespace rtype::client
