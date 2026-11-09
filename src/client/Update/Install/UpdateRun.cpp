#include "UpdateRun.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>
#include "ByteText.hpp"
#include "ChildProcess.hpp"
#include "LaunchCommand.hpp"
#include "PackDownloadOutcome.hpp"
#include "PackDownloadProgress.hpp"
#include "PackDownloadTarget.hpp"
#include "PackageDigest.hpp"
#include "PackageInstall.hpp"
#include "ReleaseConstants.hpp"
#include "ReleaseNotes.hpp"
#include "ReleaseNotesFile.hpp"
#include "ReleaseOffer.hpp"
#include "UpdateConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateProgress.hpp"
#include "UpdateRunParts.hpp"

namespace rtype::client {

namespace {

PackDownloadTarget targetOf(const ReleaseOffer& offer) {
  return PackDownloadTarget{.origin = std::string{RELEASES_ORIGIN},
                            .path = std::string{RELEASE_DOWNLOAD_PATH} +
                                    offer.versionText + RELEASE_PATH_SEPARATOR +
                                    offer.package.file,
                            .size = offer.package.size,
                            .followsRedirects = true};
}

}  // namespace

UpdateRun::UpdateRun(UpdateRunParts parts)
    : parts_(std::move(parts)),
      logger_(std::string{UPDATE_LOGGER_NAME}),
      download_(targetOf(parts_.offer)) {
  logger_.info("downloading ", parts_.offer.package.file, ", ",
               parts_.offer.package.size, " bytes");
}

UpdateProgress UpdateRun::progress() const {
  const PackDownloadProgress download = download_.progress();
  const std::scoped_lock lock{mutex_};
  return UpdateProgress{.phase = phase_,
                        .received = download.received,
                        .size = parts_.offer.package.size};
}

void UpdateRun::advance() {
  if (progress().phase != UpdatePhase::Downloading) {
    return;
  }
  const PackDownloadOutcome outcome = download_.progress().outcome;
  if (outcome == PackDownloadOutcome::Failed) {
    logger_.warn("the download of the update failed");
    end(UpdatePhase::Failed);
  } else if (outcome == PackDownloadOutcome::Finished) {
    end(UpdatePhase::Installing);
    installer_ =
        std::jthread([this, bytes = download_.takeBytes()] { install(bytes); });
  }
}

bool UpdateRun::restart() {
  std::optional<LaunchCommand> command;
  {
    const std::scoped_lock lock{mutex_};
    command = command_;
  }
  if (command.has_value() && launchDetached(*command)) {
    logger_.info("the new version starts: ", command->program.string());
    return true;
  }
  logger_.warn("the new version could not be started");
  end(UpdatePhase::Failed);
  return false;
}

void UpdateRun::install(const std::vector<std::byte>& bytes) {
  std::optional<LaunchCommand> command = installedCommand(bytes);
  if (!command.has_value()) {
    end(UpdatePhase::Failed);
    return;
  }
  const std::scoped_lock lock{mutex_};
  command_ = std::move(command);
  phase_ = UpdatePhase::ReadyToRestart;
}

std::optional<LaunchCommand> UpdateRun::installedCommand(
    std::span<const std::byte> bytes) {
  if (sha256HexadecimalOf(bytes) != parts_.offer.package.sha256) {
    logger_.warn("the package received is not the one the manifest signed");
    return std::nullopt;
  }
  const std::optional<std::filesystem::path> package = writePackage(bytes);
  if (!package.has_value() ||
      !writeReleaseNotes(parts_.notesFile,
                         ReleaseNotes{.versionText = parts_.offer.versionText,
                                      .lines = parts_.offer.notes})) {
    logger_.warn("the update could not be written down");
    return std::nullopt;
  }
  std::optional<LaunchCommand> command = installPackage(*package, parts_.place);
  if (!command.has_value()) {
    std::error_code ignored;
    std::filesystem::remove(parts_.notesFile, ignored);
    logger_.warn("the update could not replace ",
                 parts_.place.installed.string());
  }
  return command;
}

std::optional<std::filesystem::path> UpdateRun::writePackage(
    std::span<const std::byte> bytes) const {
  std::error_code error;
  std::filesystem::create_directories(parts_.downloadFolder, error);
  std::filesystem::path file =
      parts_.downloadFolder / parts_.offer.package.file;
  const std::string text = network::textOf(bytes);
  std::ofstream output{file, std::ios::binary | std::ios::trunc};
  output.write(text.data(), static_cast<std::streamsize>(text.size()));
  if (!output.flush()) {
    return std::nullopt;
  }
  return file;
}

void UpdateRun::end(UpdatePhase phase) {
  const std::scoped_lock lock{mutex_};
  phase_ = phase;
}

}  // namespace rtype::client
