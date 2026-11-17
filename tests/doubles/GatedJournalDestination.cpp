#include "GatedJournalDestination.hpp"
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include "QueuedLine.hpp"
#include "ReceivedJournalLine.hpp"

std::function<void(const rtype::logging::QueuedLine&)>
GatedJournalDestination::destination() {
  return [this](const rtype::logging::QueuedLine& line) { receive(line); };
}

void GatedJournalDestination::close() {
  const std::scoped_lock lock{mutex_};
  closed_ = true;
}

void GatedJournalDestination::open() {
  {
    const std::scoped_lock lock{mutex_};
    closed_ = false;
  }
  changed_.notify_all();
}

void GatedJournalDestination::waitForALineAtTheGate() {
  std::unique_lock lock{mutex_};
  changed_.wait(lock, [this] { return lineAtTheGate_; });
}

std::vector<ReceivedJournalLine> GatedJournalDestination::lines() const {
  const std::scoped_lock lock{mutex_};
  return lines_;
}

void GatedJournalDestination::receive(const rtype::logging::QueuedLine& line) {
  std::unique_lock lock{mutex_};
  lineAtTheGate_ = closed_;
  changed_.notify_all();
  changed_.wait(lock, [this] { return !closed_; });
  lineAtTheGate_ = false;
  lines_.push_back(ReceivedJournalLine{.text = std::string{line.text},
                                       .time = line.time,
                                       .lostBefore = line.lostBefore});
}
