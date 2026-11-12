#include "UpdateNotice.hpp"
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include "ClientException.hpp"
#include "IReleaseUpdater.hpp"
#include "IUpdateRun.hpp"
#include "ReleaseNotes.hpp"
#include "ReleaseNotesFile.hpp"
#include "ReleaseOffer.hpp"
#include "UpdateCheck.hpp"
#include "UpdateConstants.hpp"
#include "UpdatePanelConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateRun.hpp"
#include "UpdateRunParts.hpp"

namespace rtype::client {

UpdateNotice::UpdateNotice(
    std::function<std::unique_ptr<IReleaseUpdater>()> makeUpdater,
    std::filesystem::path notesFile)
    : makeUpdater_(std::move(makeUpdater)),
      notesFile_(std::move(notesFile)),
      logger_(std::string{UPDATE_LOGGER_NAME}) {}

UpdateNotice::~UpdateNotice() { stop(); }

bool UpdateNotice::updateAvailable() const {
  if (run_ != nullptr) {
    return run_->progress().phase != UpdatePhase::Failed;
  }
  return check_ != nullptr && check_->found();
}

bool UpdateNotice::notesWaiting() const { return notes_.has_value(); }

void UpdateNotice::start() {
  try {
    updater_ = makeUpdater_();
  } catch (const UpdateException& error) {
    logger_.info(error.what(), ": no update");
    return;
  }
  currentVersion_ = updater_->currentVersion();
  readNotes();
  check_ = std::make_unique<UpdateCheck>(*updater_);
}

void UpdateNotice::stop() {
  if (updater_ != nullptr) {
    updater_->stop();
  }
  run_.reset();
  check_.reset();
}

IUpdateRun& UpdateNotice::run() {
  if (run_ == nullptr) {
    run_ = std::make_unique<UpdateRun>(
        UpdateRunParts{.updater = *updater_,
                       .offer = check_->offer().value_or(ReleaseOffer{}),
                       .notesFile = notesFile_});
  }
  return *run_;
}

std::string UpdateNotice::versionLine() const {
  return std::string{VERSION_PREFIX} + currentVersion_ +
         std::string{VERSION_ARROW} +
         check_->offer().value_or(ReleaseOffer{}).versionText;
}

ReleaseNotes UpdateNotice::notes() const {
  return notes_.value_or(ReleaseNotes{});
}

void UpdateNotice::markNotesShown() {
  notes_.reset();
  std::error_code ignored;
  std::filesystem::remove(notesFile_, ignored);
}

void UpdateNotice::readNotes() {
  std::optional<ReleaseNotes> notes = readReleaseNotes(notesFile_);
  if (notes.has_value() && notes->versionText == currentVersion_ &&
      !notes->lines.empty()) {
    notes_ = std::move(notes);
    return;
  }
  std::error_code ignored;
  std::filesystem::remove(notesFile_, ignored);
}

}  // namespace rtype::client
