#include <spawn.h>
// NOLINTNEXTLINE(misc-include-cleaner): POSIX declares pid_t here.
#include <sys/types.h>
#include <sys/wait.h>
#include <array>
#include <string>
#include <thread>
#include "BrowserOpening.hpp"
#include "PlatformConstants.hpp"
#if defined(__APPLE__)
#include <crt_externs.h>
#else
#include <unistd.h>
#endif

namespace rtype::client {

namespace {

/** @returns The environment of the program, which the opener is started with
 * so it finds the display of the player: glibc declares it in <unistd.h>, and
 * macOS hands it out through _NSGetEnviron(). */
char** programEnvironment() {
#if defined(__APPLE__)
  return *_NSGetEnviron();
#else
  return environ;
#endif
}

}  // namespace

bool openInBrowser(const std::string& url) {
  if (!isWebAddress(url)) {
    return false;
  }
  std::string command{BROWSER_OPENER_COMMAND};
  std::string address = url;
  std::array<char*, 3> arguments{command.data(), address.data(), nullptr};
  // NOLINTNEXTLINE(misc-include-cleaner)
  pid_t opener = 0;
  if (posix_spawnp(&opener, command.c_str(), nullptr, nullptr, arguments.data(),
                   programEnvironment()) != 0) {
    return false;
  }
  std::thread([opener] {
    int status = 0;
    static_cast<void>(waitpid(opener, &status, 0));
  }).detach();
  return true;
}

}  // namespace rtype::client
