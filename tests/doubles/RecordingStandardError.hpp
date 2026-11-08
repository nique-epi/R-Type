#pragma once

#include <mutex>
#include <string>
#include <vector>

/** @brief Takes what the journal writes to standard error, through
 * logging::setStandardErrorWriter(), from its construction to its destruction,
 * when it gives standard error back to std::cerr. Lines may come from several
 * threads, as from the relays of a child process. */
class RecordingStandardError {
 public:
  RecordingStandardError();
  ~RecordingStandardError();

  RecordingStandardError(const RecordingStandardError&) = delete;
  RecordingStandardError& operator=(const RecordingStandardError&) = delete;
  RecordingStandardError(RecordingStandardError&&) = delete;
  RecordingStandardError& operator=(RecordingStandardError&&) = delete;

  /** @returns The lines received so far, in order. */
  [[nodiscard]] std::vector<std::string> lines() const;

 private:
  mutable std::mutex mutex_;
  std::vector<std::string> lines_;
};
