#pragma once

namespace rtype::server {

/**
 * @brief How a process ended: what its main returned or, on Linux and macOS
 * when a signal killed it, the number of that signal (9 for SIGKILL). A process
 * that ended normally has code 0.
 */
struct ProcessExit {
  int code{};
};

}  // namespace rtype::server
