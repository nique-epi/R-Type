#pragma once

#include <boost/asio/io_context.hpp>
#include <memory>
#include "ChildBinding.hpp"
#include "IChildProcess.hpp"
#include "IProcessLauncher.hpp"
#include "ProcessCommand.hpp"

namespace rtype::server {

/**
 * @brief Starts child processes with Boost.Process v2. Its io_context never
 * runs: Boost.Process needs one to build a process, and the children are polled
 * (see BoostChildProcess), so the launcher creates no thread.
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
  boost::asio::io_context context_;
  ChildBinding binding_;
};

}  // namespace rtype::server
