#include "UpdateNotice.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include "ClientReleaseConstants.hpp"
#include "ExecutablePath.hpp"
#include "IUpdateRun.hpp"
#include "InstallConstants.hpp"
#include "PackageInstall.hpp"
#include "ReleaseNotesFile.hpp"
#include "ReleaseVersion.hpp"
#include "SettingsFolder.hpp"
#include "UpdateConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateRunParts.hpp"
#include "UpdateScreenConstants.hpp"

namespace rtype::client {

namespace {

std::filesystem::path downloadFolder() {
  std::error_code error;
  return std::filesystem::temp_directory_path(error) / DOWNLOAD_FOLDER_NAME;
}

}  // namespace

UpdateNotice::UpdateNotice() : logger_(std::string{UPDATE_LOGGER_NAME}) {}

bool UpdateNotice::updateAvailable() const {
  if (run_ != nullptr) {
    return run_->progress().phase != UpdatePhase::Failed;
  }
  return check_ != nullptr && check_->found();
}

bool UpdateNotice::notesWaiting() const { return notes_.has_value(); }

void UpdateNotice::start() {
  readNotes();
  place_ = installPlace();
  const std::optional<ReleaseVersion> current =
      releaseVersionOf(CLIENT_RELEASE_VERSION);
  if (!place_.has_value() || !current.has_value()) {
    logger_.info("not run from an installed package: no update");
    return;
  }
  removeLeftovers(*place_);
  std::error_code ignored;
  std::filesystem::remove_all(downloadFolder(), ignored);
  check_ = std::make_unique<ReleaseCheck>(place_->system, *current);
}

void UpdateNotice::stop() {
  run_.reset();
  check_.reset();
}

IUpdateRun& UpdateNotice::run() {
  if (run_ == nullptr) {
    run_ = std::make_unique<UpdateRun>(
        UpdateRunParts{.offer = check_->offer().value(),
                       .place = place_.value(),
                       .downloadFolder = downloadFolder(),
                       .notesFile = notesFile_});
  }
  return *run_;
}

std::string UpdateNotice::versionLine() const {
  return std::string{VERSION_PREFIX} + std::string{CLIENT_RELEASE_VERSION} +
         std::string{VERSION_ARROW} + check_->offer().value().versionText;
}

ReleaseNotes UpdateNotice::notes() const { return notes_.value(); }

void UpdateNotice::markNotesShown() {
  notes_.reset();
  std::error_code ignored;
  std::filesystem::remove(notesFile_, ignored);
}

void UpdateNotice::readNotes() {
  notesFile_ = settingsFolder().value_or(engine::executableFolder()) /
               RELEASE_NOTES_FILE_NAME;
  std::optional<ReleaseNotes> notes = readReleaseNotes(notesFile_);
  if (notes.has_value() && notes->versionText == CLIENT_RELEASE_VERSION &&
      !notes->lines.empty()) {
    notes_ = std::move(notes);
    return;
  }
  std::error_code ignored;
  std::filesystem::remove(notesFile_, ignored);
}

}  // namespace rtype::client
