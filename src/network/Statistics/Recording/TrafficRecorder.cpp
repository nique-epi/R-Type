#include "TrafficRecorder.hpp"
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include "TrafficCounters.hpp"
#include "TrafficCsvRows.hpp"
#include "TrafficLine.hpp"
#include "TrafficRecordingConstants.hpp"
#include "TrafficTotals.hpp"

namespace rtype::network {

TrafficRecorder::TrafficRecorder(
    const TrafficCounters& counters,
    const std::optional<std::filesystem::path>& csvFile,
    std::chrono::nanoseconds now)
    : counters_(&counters),
      logger_(std::string{TRAFFIC_LOGGER_NAME}),
      start_(now),
      lastRows_(now),
      lastLine_(now),
      lastTotals_(trafficTotalsOf(counters.report())) {
  if (csvFile.has_value()) {
    file_.emplace(*csvFile, TRAFFIC_CSV_HEADER);
  }
}

void TrafficRecorder::advance(std::chrono::nanoseconds now) {
  if (now - lastRows_ >= TRAFFIC_CSV_PERIOD) {
    writeRows(now);
  }
  if (now - lastLine_ >= TRAFFIC_LINE_PERIOD) {
    writeLine(now);
  }
}

void TrafficRecorder::finish(std::chrono::nanoseconds now) { writeRows(now); }

void TrafficRecorder::writeRows(std::chrono::nanoseconds now) {
  lastRows_ = now;
  if (!file_.has_value()) {
    return;
  }
  if (!file_->append(trafficCsvRowsOf(now - start_, counters_->report()))) {
    logger_.warn("the traffic file cannot be written: no more rows will be");
    file_.reset();
  }
}

void TrafficRecorder::writeLine(std::chrono::nanoseconds now) {
  const TrafficTotals totals = trafficTotalsOf(counters_->report());
  logger_.info(trafficLineOf(lastTotals_, totals, now - lastLine_));
  lastTotals_ = totals;
  lastLine_ = now;
}

}  // namespace rtype::network
