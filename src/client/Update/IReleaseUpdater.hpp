#pragma once

#include <functional>
#include <optional>
#include <string>
#include "ReleaseOffer.hpp"

namespace rtype::client {

/**
 * @brief The updates of the installed client: what the update module asks
 * Velopack, behind an interface the tests replace.
 *
 * newerRelease() runs on the thread of the check, download() on the thread of
 * the update, applyOnExit() on the frame loop; they follow one another, never
 * overlap, and stop() may come from the frame loop at any time.
 */
class IReleaseUpdater {
 public:
  virtual ~IReleaseUpdater() = default;

  /** @returns The version installed, as Velopack writes it. */
  [[nodiscard]] virtual std::string currentVersion() = 0;

  /** @returns The newer release the signed feed offers, or nothing.
   * @throws UpdateException when the feed could not be read or trusted. */
  [[nodiscard]] virtual std::optional<ReleaseOffer> newerRelease() = 0;

  /** @brief Downloads the package of the release newerRelease() found;
   * @p progress receives the share received, from 0 to
   * FULL_DOWNLOAD_PERCENT.
   * @throws UpdateException when it was not downloaded, or is not the one the
   * feed signed. */
  virtual void download(const std::function<void(int)>& progress) = 0;

  /** @brief Starts the updater of Velopack, which waits for the client to
   * exit, replaces it with the release downloaded and starts it.
   * @throws UpdateException when it could not start. */
  virtual void applyOnExit() = 0;

  /** @brief Ends a check or a download in progress, so its thread can be
   * joined at once. */
  virtual void stop() = 0;
};

}  // namespace rtype::client
