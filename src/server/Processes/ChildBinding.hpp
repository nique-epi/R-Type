#pragma once

#include <boost/process/v2/default_launcher.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/system/detail/error_code.hpp>
#include <filesystem>

namespace rtype::server {

/**
 * @brief Ties the life of the processes a launcher starts to the life of the
 * program that started them: when that program ends, however it ends, they end
 * with it. Pass it to Boost.Process v2 as an initializer, after the arguments
 * (Linux and macOS), then call bind() on the process that was started.
 *
 * Windows: the constructor creates a job object with
 * JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE; every process started is assigned to it,
 * and the system kills them all when the last handle of the job closes, which
 * is when this program ends. Linux: each child asks for SIGTERM when its
 * parent dies (PR_SET_PDEATHSIG) between fork and exec; the "parent" is the
 * thread that started the child, so the launcher must be used from a thread
 * that lives as long as the program. macOS: nothing here (the system has no
 * such call); a game process stops by itself when its supervisor goes silent.
 *
 * The hook name on_exec_setup is the one Boost.Process v2 looks for; it cannot
 * follow the naming rules of the repository. On Windows the job is not an
 * initializer: Boost.Process v2 only recognizes an on_success hook that returns
 * an error_code, and then leaves its function without returning a value, which
 * MSVC rejects; bind() assigns the process to the job after it started.
 */
class ChildBinding {
 public:
  /** @throws ChildBindingException when the system refuses the job object
   * (Windows only). */
  ChildBinding();
  // NOLINTNEXTLINE(performance-trivially-destructible)
  ~ChildBinding();

  ChildBinding(const ChildBinding&) = delete;
  ChildBinding& operator=(const ChildBinding&) = delete;
  ChildBinding(ChildBinding&&) = delete;
  ChildBinding& operator=(ChildBinding&&) = delete;

#if !defined(_WIN32)
  /** @brief Called by Boost.Process in the child, between fork and exec: asks
   * for SIGTERM when the parent dies (Linux). A returned error stops the exec
   * and makes the launch fail. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  boost::system::error_code on_exec_setup(
      boost::process::v2::default_process_launcher& launcher,
      const std::filesystem::path& program, const char* const* arguments);
#endif

  /**
   * @brief Ties @p process, which just started, to the program: it joins the
   * job object on Windows; nothing to do on Linux and macOS, where the
   * initializer did it in the child.
   * @throws ChildBindingException when the system refuses the job; the process
   * is terminated first.
   */
  void bind(boost::process::v2::process& process);

 private:
#if defined(_WIN32)
  void* job_{nullptr};
#endif
};

}  // namespace rtype::server
