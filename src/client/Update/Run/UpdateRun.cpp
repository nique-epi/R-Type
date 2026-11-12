#include "UpdateRun.hpp"
#include <filesystem>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include "ClientException.hpp"
#include "NoteLines.hpp"
#include "ReleaseNotes.hpp"
#include "ReleaseNotesFile.hpp"
#include "UpdateConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateProgress.hpp"
#include "UpdateRunParts.hpp"

namespace rtype::client {

UpdateRun::UpdateRun(UpdateRunParts parts)
    : parts_(std::move(parts)),
      logger_(std::string{UPDATE_LOGGER_NAME}),
      thread_([this] { download(); }) {}

UpdateProgress UpdateRun::progress() const {
  return UpdateProgress{.phase = phase_.load(), .percent = percent_.load()};
}

bool UpdateRun::restart() {
  try {
    parts_.updater.get().applyOnExit();
  } catch (const UpdateException& error) {
    logger_.warn(error.what());
    std::error_code ignored;
    std::filesystem::remove(parts_.notesFile, ignored);
    phase_.store(UpdatePhase::Failed);
    return false;
  }
  logger_.info("the updater replaces the client once it exits");
  return true;
}

void UpdateRun::download() {
  logger_.info("downloading release ", parts_.offer.versionText);
  try {
    parts_.updater.get().download(
        [this](int percent) { percent_.store(percent); });
  } catch (const UpdateException& error) {
    logger_.warn(error.what());
    phase_.store(UpdatePhase::Failed);
    return;
  }
  phase_.store(UpdatePhase::Installing);
  if (!writeReleaseNotes(
          parts_.notesFile,
          ReleaseNotes{.versionText = parts_.offer.versionText,
                       .lines = noteLinesOf(parts_.offer.notesMarkdown)})) {
    logger_.warn("the notes of the release could not be kept");
  }
  phase_.store(UpdatePhase::ReadyToRestart);
}

}  // namespace rtype::client
