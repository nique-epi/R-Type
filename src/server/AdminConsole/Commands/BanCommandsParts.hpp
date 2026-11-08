#pragma once

#include <chrono>
#include <functional>
#include "BanBook.hpp"
#include "IConsoleOutput.hpp"

namespace rtype::server {

/** @brief What the commands of the bans change and write to; each must
 * outlive the commands. */
struct BanCommandsParts {
  /** @brief The bans of the supervisor. */
  std::reference_wrapper<BanBook> book;
  /** @brief Where the answers go. */
  std::reference_wrapper<IConsoleOutput> output;
  /** @brief The time now, to the second: currentSecond() for the console, a
   * fixed moment for the tests. Must not be empty. */
  std::function<std::chrono::sys_seconds()> now;
};

}  // namespace rtype::server
