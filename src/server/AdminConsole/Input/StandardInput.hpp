#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <stop_token>
#include "IConsoleInput.hpp"

namespace rtype::server {

/**
 * @brief The standard input of the supervisor, as the console reads it.
 *
 * On Linux and macOS, select() waits on the input and on a pipe that the stop
 * writes one byte to (the self-pipe trick); select() rather than poll(), which
 * macOS does not support on a terminal. SIGTTIN is ignored during the read, as
 * ioquake3 does, so a supervisor run in the background of a shell is not
 * suspended when it reads its terminal: the read fails with EIO, and the
 * console tries again CONSOLE_BACKGROUND_WAIT later, waiting on the pipe so the
 * stop still wakes it, until the shell brings it back to the foreground.
 *
 * On Windows, ReadFile() waits on the input, a console or a pipe, and the stop
 * cancels it with CancelSynchronousIo(); Ctrl+C ends a console read with
 * nothing read, and the read starts again. That file is checked by the Windows
 * job of the CI only.
 */
class StandardInput final : public IConsoleInput {
 public:
  [[nodiscard]] std::optional<std::size_t> read(
      std::span<char> buffer, const std::stop_token& stop) override;
};

}  // namespace rtype::server
