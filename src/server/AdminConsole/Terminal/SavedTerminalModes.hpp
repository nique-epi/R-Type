#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#endif

namespace rtype::server {

/** @brief The modes the terminal had before
 * StandardTerminal::enterTypingMode(), which leaveTypingMode() gives back. */
struct SavedTerminalModes {
#ifdef _WIN32
  DWORD input{};
  DWORD output{};
#else
  termios input{};
#endif
};

}  // namespace rtype::server
