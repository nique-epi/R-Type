#pragma once

#include <mutex>
#include <optional>
#include <thread>
#include "IReleaseUpdater.hpp"
#include "Logger.hpp"
#include "ReleaseOffer.hpp"

namespace rtype::client {

/**
 * @brief The check of the latest release, on a thread of its own: building it
 * starts the check, destroying it waits for the check, which the owner ends
 * first with IReleaseUpdater::stop().
 *
 * A failure of the check offers nothing, and is logged: the title shows no
 * panel.
 */
class UpdateCheck {
 public:
  /** @param updater What is checked; it must outlive the check. */
  explicit UpdateCheck(IReleaseUpdater& updater);

  /** @returns Whether a newer release was found; cheap, read at each frame. */
  [[nodiscard]] bool found() const;

  /** @returns The release found, or nothing before. */
  [[nodiscard]] std::optional<ReleaseOffer> offer() const;

 private:
  void run();

  IReleaseUpdater* updater_;
  logging::Logger logger_;
  mutable std::mutex mutex_;
  std::optional<ReleaseOffer> offer_;
  std::jthread thread_;
};

}  // namespace rtype::client
