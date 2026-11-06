#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "IChildProcess.hpp"
#include "IProcessLauncher.hpp"
#include "ProcessCommand.hpp"
#include "RecordingChildState.hpp"

/** @brief A launcher that starts no process: it writes down each command and
 * hands out children a test makes end. */
class RecordingProcessLauncher final : public rtype::server::IProcessLauncher {
 public:
  std::unique_ptr<rtype::server::IChildProcess> launch(
      rtype::server::ProcessCommand command) override;

  /** @returns The commands launched, oldest first; a launch that failed is not
   * in the list. */
  [[nodiscard]] const std::vector<rtype::server::ProcessCommand>& commands()
      const;

  /** @brief Has the child launched at @p index end with @p code. */
  void exitChild(std::size_t index, int code);

  /** @returns Whether the child at @p index was terminated. */
  [[nodiscard]] bool childWasTerminated(std::size_t index) const;

  /** @brief Makes the next launch throw ProcessLaunchException. */
  void failNextLaunch();

 private:
  std::vector<rtype::server::ProcessCommand> commands_;
  std::vector<std::shared_ptr<RecordingChildState>> states_;
  bool failNext_{false};
};
