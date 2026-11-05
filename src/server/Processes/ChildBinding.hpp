#pragma once

#include <boost/process/v2/default_launcher.hpp>
#include <boost/system/error_code.hpp>
#include <filesystem>
#include <string>

namespace rtype::server {

/**
 * @brief Ties the life of the processes a launcher starts to the life of the
 * program that started them: when that program ends, however it ends, they end
 * with it. Pass it to Boost.Process v2 as an initializer, after the arguments.
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
 * The hook names (on_success, on_exec_setup) are the ones Boost.Process v2
 * looks for; they cannot follow the naming rules of the repository.
 */
class ChildBinding {
 public:
  /** @throws ChildBindingException when the system refuses the job object
   * (Windows only). */
  ChildBinding();
  ~ChildBinding();

  ChildBinding(const ChildBinding&) = delete;
  ChildBinding& operator=(const ChildBinding&) = delete;
  ChildBinding(ChildBinding&&) = delete;
  ChildBinding& operator=(ChildBinding&&) = delete;

#if defined(_WIN32)
  /** @brief Called by Boost.Process after CreateProcess: assigns the new
   * process to the job. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void on_success(boost::process::v2::default_process_launcher& launcher,
                  const std::filesystem::path& program,
                  std::wstring& commandLine);
#else
  /** @brief Called by Boost.Process in the child, between fork and exec: asks
   * for SIGTERM when the parent dies (Linux). A returned error stops the exec
   * and makes the launch fail. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  boost::system::error_code on_exec_setup(
      boost::process::v2::default_process_launcher& launcher,
      const std::filesystem::path& program, const char* const* arguments);
#endif

  /** @returns Whether the process started last was tied to the parent; always
   * true outside Windows. */
  [[nodiscard]] bool lastProcessWasBound() const;

 private:
#if defined(_WIN32)
  void* job_{nullptr};
  bool lastProcessBound_{true};
#endif
};

}  // namespace rtype::server
