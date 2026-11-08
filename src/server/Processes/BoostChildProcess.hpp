#pragma once

#include <boost/process/v2/process.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include "ChildOutputRelay.hpp"
#include "IChildProcess.hpp"
#include "ProcessExit.hpp"

namespace rtype::server {

/**
 * @brief A child process started by Boost.Process v2. It is polled, never
 * waited for: exited() asks the system without blocking (waitpid with WNOHANG
 * on Linux and macOS, GetExitCodeProcess on Windows), so no thread waits for
 * it. The relays of its outputs read them on their own threads.
 */
class BoostChildProcess final : public IChildProcess {
 public:
  /** @param relays The relays of the outputs of @p process, which end once it
   * ended. */
  BoostChildProcess(boost::process::v2::process process,
                    std::vector<std::unique_ptr<ChildOutputRelay>> relays);

  /** @brief Kills the process if it still runs, and collects it, so it never
   * stays a zombie; then the relays, which the end of the process ends, are
   * joined. */
  ~BoostChildProcess() override;

  BoostChildProcess(const BoostChildProcess&) = delete;
  BoostChildProcess& operator=(const BoostChildProcess&) = delete;
  BoostChildProcess(BoostChildProcess&&) = delete;
  BoostChildProcess& operator=(BoostChildProcess&&) = delete;

  void terminate() override;
  [[nodiscard]] std::optional<ProcessExit> exited() override;
  [[nodiscard]] std::int64_t processId() const override;

 private:
  boost::process::v2::process process_;
  std::vector<std::unique_ptr<ChildOutputRelay>> relays_;
};

}  // namespace rtype::server
