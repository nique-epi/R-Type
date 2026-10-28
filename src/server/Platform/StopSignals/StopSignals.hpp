#pragma once

#include <atomic>

namespace rtype::server {

/**
 * @brief While it lives, records that the program was asked to stop, instead
 * of letting the system end it: Ctrl-C and SIGTERM on Linux and macOS; Ctrl-C,
 * Ctrl-Break and the closing of the console on Windows. Destroying it gives
 * them back to the system, which ends the program again.
 *
 * On Linux and macOS it installs a std::signal handler for SIGINT and
 * SIGTERM; glibc and macOS keep it installed after a signal and restart the
 * system calls the signal interrupts (Linux man page signal(2), BSD
 * semantics).
 * Windows generates no SIGTERM and runs a SIGINT handler on a thread of its
 * own (Microsoft, "signal"), so there it adds a console control handler
 * (SetConsoleCtrlHandler) for CTRL_C_EVENT, CTRL_BREAK_EVENT and
 * CTRL_CLOSE_EVENT, the closing of the console being how Windows asks a
 * console program to end. After CTRL_CLOSE_EVENT, Windows ends the process
 * once the handler returns, or 5 s after the event (Microsoft,
 * "HandlerRoutine"): on that event the handler records the request then
 * waits, so the program stops by itself and the process ends when main()
 * returns, or at the end of those 5 s.
 *
 * Each handler only stores into a lock-free flag, which is all a signal
 * handler may do. Build one at a time: they share that flag. Giving the
 * signals back is not checked: the system only refuses it for a signal number
 * or a handler it does not know, and the destructor removes exactly what the
 * constructor installed.
 */
class StopSignals {
 public:
  /**
   * @brief Clears the flag, then installs the handlers.
   * @throws StopSignalsRefusedException when the system refuses a handler; the
   * ones already installed are removed first.
   */
  StopSignals();

  /** @brief Gives the signals back to the system's default handling. */
  ~StopSignals();

  StopSignals(const StopSignals&) = delete;
  StopSignals& operator=(const StopSignals&) = delete;
  StopSignals(StopSignals&&) = delete;
  StopSignals& operator=(StopSignals&&) = delete;

  /**
   * @returns Whether a stop was asked for since the object was built. Safe
   * from any thread.
   */
  [[nodiscard]] bool received() const;

 private:
  const std::atomic<bool>* requested_;
};

}  // namespace rtype::server
