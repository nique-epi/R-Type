#pragma once

#include <atomic>
#include <cstddef>
#include <mutex>
#include <optional>
#include <string_view>
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "ThrottledLog.hpp"

namespace rtype::server {

/**
 * @brief Counts and logs the datagrams the DatagramSorter drops before any
 * queue, those dropped for their session token apart from the others. Each
 * drop is logged at the debug level with its sender and why, at most once per
 * REJECTION_LINE_INTERVAL (see ThrottledLog). Call drop() and dropWrongToken()
 * on the network thread only, and the other functions from any thread.
 */
class DatagramDrops {
 public:
  /** @param clock The time of the lines; it must outlive this object. */
  explicit DatagramDrops(const engine::IClock& clock);

  /** @brief Counts and logs one datagram of @p sender dropped for @p reason. */
  void drop(const network::Endpoint& sender, std::string_view reason);

  /**
   * @brief Counts and logs one datagram of @p sender dropped for its session
   * token, and keeps @p sender when it is the first since the last call to
   * takeWrongTokenSender().
   */
  void dropWrongToken(const network::Endpoint& sender);

  /**
   * @returns How many datagrams drop() counted since this object was built.
   */
  [[nodiscard]] std::size_t droppedCount() const;

  /**
   * @returns How many datagrams dropWrongToken() counted since this object
   * was built.
   */
  [[nodiscard]] std::size_t wrongTokenCount() const;

  /**
   * @returns The sender of the first datagram dropped for its session token
   * since the previous call, or nothing when none was; the next call starts
   * afresh. The sender is kept before the drop is counted, so a call made
   * after wrongTokenCount() grew finds it, unless an earlier call took it in
   * between.
   */
  [[nodiscard]] std::optional<network::Endpoint> takeWrongTokenSender();

 private:
  /** @brief Logs one datagram of @p sender dropped for @p reason. */
  void logDrop(const network::Endpoint& sender, std::string_view reason);

  const engine::IClock* clock_;
  std::atomic<std::size_t> droppedCount_{0};
  std::atomic<std::size_t> wrongTokenCount_{0};
  std::mutex wrongTokenSenderMutex_;
  std::optional<network::Endpoint> wrongTokenSender_;
  logging::ThrottledLog dropLog_;
};

}  // namespace rtype::server
