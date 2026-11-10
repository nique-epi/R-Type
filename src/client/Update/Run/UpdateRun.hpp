#pragma once

#include <atomic>
#include <thread>
#include "IUpdateRun.hpp"
#include "Logger.hpp"
#include "UpdatePhase.hpp"
#include "UpdateProgress.hpp"
#include "UpdateRunParts.hpp"

namespace rtype::client {

/**
 * @brief An update: building it starts the download of the release on a
 * thread of its own; once the package is the one the feed lists, it
 * writes the notes of the release for the next start and is ready to
 * restart. restart() starts the updater of Velopack, which replaces the
 * client once it exits and starts the new version.
 *
 * Destroying it waits for the download, which the owner ends first with
 * IReleaseUpdater::stop().
 */
class UpdateRun final : public IUpdateRun {
 public:
  explicit UpdateRun(UpdateRunParts parts);

  [[nodiscard]] UpdateProgress progress() const override;
  /** @returns Whether the updater started, the client then to close; when it
   * did not, the update failed and the notes are removed. */
  [[nodiscard]] bool restart() override;

 private:
  void download();

  UpdateRunParts parts_;
  logging::Logger logger_;
  std::atomic<UpdatePhase> phase_{UpdatePhase::Downloading};
  std::atomic<int> percent_{0};
  std::jthread thread_;
};

}  // namespace rtype::client
