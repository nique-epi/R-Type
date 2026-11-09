#pragma once

#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include "Logger.hpp"
#include "ReleaseOffer.hpp"
#include "ReleaseVersion.hpp"

namespace rtype::client {

/**
 * @brief The check of the latest release, on a thread of its own: building it
 * starts the check, destroying it stops it, within RELEASE_CHECK_TIMEOUT.
 *
 * It reads the manifest of the latest release, then its signature, and keeps
 * what it offers the system only when the signature is the key of the game's
 * and the version is newer than the current one. A failure at any step offers
 * nothing, and is logged.
 */
class ReleaseCheck {
 public:
  /** @param system Key of the package of this system in the manifest. */
  ReleaseCheck(std::string system, ReleaseVersion current);

  /** @returns Whether a newer release was found; cheap, read at each frame. */
  [[nodiscard]] bool found() const;

  /** @returns The release found, or nothing before. */
  [[nodiscard]] std::optional<ReleaseOffer> offer() const;

 private:
  void run(const std::stop_token& stop);
  [[nodiscard]] std::optional<ReleaseOffer> newerOffer(
      const std::stop_token& stop);

  std::string system_;
  ReleaseVersion current_;
  logging::Logger logger_;
  mutable std::mutex mutex_;
  std::optional<ReleaseOffer> offer_;
  std::jthread thread_;
};

}  // namespace rtype::client
