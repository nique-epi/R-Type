#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>
#include "QueuedLine.hpp"
#include "ReceivedJournalLine.hpp"

/**
 * @brief A destination of a JournalWriter that keeps what reaches it and that a
 * test may close like a gate: while it is closed, the thread of the journal
 * waits inside it, as on a disk that does not answer, and the lines queued
 * meanwhile pile up in the queue.
 */
class GatedJournalDestination {
 public:
  /** @returns The function a JournalWriter calls; it must not outlive this. */
  [[nodiscard]] std::function<void(const rtype::logging::QueuedLine&)>
  destination();

  /** @brief Makes the next line wait inside the destination until open(). */
  void close();

  /** @brief Lets every waiting line through, and the next ones. */
  void open();

  /** @brief Waits until a line waits at the closed gate. */
  void waitForALineAtTheGate();

  /** @returns The lines received so far, in order. */
  [[nodiscard]] std::vector<ReceivedJournalLine> lines() const;

 private:
  void receive(const rtype::logging::QueuedLine& line);

  mutable std::mutex mutex_;
  std::condition_variable changed_;
  bool closed_{false};
  bool lineAtTheGate_{false};
  std::vector<ReceivedJournalLine> lines_;
};
