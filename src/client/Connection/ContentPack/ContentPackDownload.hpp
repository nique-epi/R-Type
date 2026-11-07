#pragma once

#include <cstddef>
#include <mutex>
#include <span>
#include <stop_token>
#include <thread>
#include <vector>
#include "PackDownloadProgress.hpp"
#include "PackDownloadTarget.hpp"
#include "PackTryEnd.hpp"

namespace rtype::client {

/**
 * @brief The download of a content pack over HTTP, on a thread of its own:
 * the frame loop reads how far it went and never waits on it.
 *
 * It asks the path of the target, then, when a try is cut, asks again for
 * the bytes from where it stopped (a range, RFC 9110), PACK_DOWNLOAD_ATTEMPTS
 * tries in all. A server that answers the range with the whole pack starts it
 * again. It never keeps more bytes than the size of the target: a server that
 * sends more, or answers another status than 200 or 206, fails the download
 * at once. Building it starts the thread; destroying it
 * stops the thread, a try in progress ending within PACK_TRANSFER_TIMEOUT.
 */
class ContentPackDownload {
 public:
  explicit ContentPackDownload(PackDownloadTarget target);

  /** @returns How far the download went; safe from any thread. */
  [[nodiscard]] PackDownloadProgress progress() const;

  /** @returns The bytes received, once the outcome is Finished; empty before.
   */
  [[nodiscard]] std::vector<std::byte> takeBytes();

 private:
  /** @brief Makes the tries until one ends the pack or none is left. */
  void run(const std::stop_token& stop);

  /** @returns How a try from the bytes received so far ended. */
  [[nodiscard]] PackTryEnd tryOnce(const std::stop_token& stop);

  /** @brief Adds the bytes a try received, unless they pass the size.
   * @returns Whether they were kept. */
  [[nodiscard]] bool keep(std::span<const char> data);

  void finish(PackDownloadOutcome outcome);

  PackDownloadTarget target_;
  mutable std::mutex mutex_;
  std::vector<std::byte> bytes_;
  PackDownloadOutcome outcome_{PackDownloadOutcome::Running};
  std::jthread thread_;
};

}  // namespace rtype::client
