#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include "Logger.hpp"
#include "TrafficCounters.hpp"
#include "TrafficCsvFile.hpp"
#include "TrafficTotals.hpp"

namespace rtype::network {

/**
 * @brief Writes the traffic of a program as time goes by: its rows to a file
 * every TRAFFIC_CSV_PERIOD, when it was given one, and a line to the journal
 * every TRAFFIC_LINE_PERIOD.
 *
 * Call it from a thread that may wait on a disk, never from the network thread.
 * A file that stops accepting rows is told once in the journal and left, so a
 * full disk never stops a game.
 */
class TrafficRecorder {
 public:
  /**
   * @param counters What is read; it must outlive the recorder.
   * @param csvFile Where the rows go; nothing writes no rows.
   * @param now The start, on the clock advance() is given.
   * @throws TrafficFileOpenException when @p csvFile cannot be opened.
   */
  TrafficRecorder(const TrafficCounters& counters,
                  const std::optional<std::filesystem::path>& csvFile,
                  std::chrono::nanoseconds now);

  /** @brief Writes what is due at @p now, if anything. */
  void advance(std::chrono::nanoseconds now);

  /** @brief Writes the rows of @p now, whenever the last ones were: the end of
   * a run, which would otherwise miss up to TRAFFIC_CSV_PERIOD. */
  void finish(std::chrono::nanoseconds now);

 private:
  void writeRows(std::chrono::nanoseconds now);
  void writeLine(std::chrono::nanoseconds now);

  const TrafficCounters* counters_;
  logging::Logger logger_;
  std::optional<TrafficCsvFile> file_;
  std::chrono::nanoseconds start_;
  std::chrono::nanoseconds lastRows_;
  std::chrono::nanoseconds lastLine_;
  TrafficTotals lastTotals_;
};

}  // namespace rtype::network
