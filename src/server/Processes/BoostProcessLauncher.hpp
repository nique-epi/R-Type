#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <memory>
#include "ChildBinding.hpp"
#include "IChildProcess.hpp"
#include "IProcessLauncher.hpp"
#include "ProcessCommand.hpp"

namespace rtype::server {

/**
 * @brief Starts child processes with Boost.Process v2. Its io_context never
 * runs: Boost.Process needs one to build a process and its pipes, and the
 * children are polled (see BoostChildProcess). The standard output and error
 * of each child go to two pipes, each read by a ChildOutputRelay on a thread
 * of its own, which writes their lines to the standard error of this process;
 * its standard input is inherited. The launcher must outlive its children.
 *
 * Start every child from the same long-lived thread (the simulation thread of
 * the supervisor): on Linux a child dies when the thread that started it ends.
 */
class BoostProcessLauncher final : public IProcessLauncher {
 public:
  BoostProcessLauncher();
  ~BoostProcessLauncher() override;

  BoostProcessLauncher(const BoostProcessLauncher&) = delete;
  BoostProcessLauncher& operator=(const BoostProcessLauncher&) = delete;
  BoostProcessLauncher(BoostProcessLauncher&&) = delete;
  BoostProcessLauncher& operator=(BoostProcessLauncher&&) = delete;

  [[nodiscard]] std::unique_ptr<IChildProcess> launch(
      ProcessCommand command) override;

 private:
  /**
   * @returns The process @p command describes, its outputs given by @p stdio,
   * started from the io_context. On Linux and macOS the binding is an
   * initializer of the launch; on Windows it joins the process to its job
   * afterwards (see ChildBinding). @p stdio closes the copies this process
   * holds of the ends the child writes to as it leaves.
   */
  [[nodiscard]] boost::process::v2::process startProcess(
      const ProcessCommand& command, boost::process::v2::process_stdio stdio);

  boost::asio::io_context context_;
  ChildBinding binding_;
};

}  // namespace rtype::server
