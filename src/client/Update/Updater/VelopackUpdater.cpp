#include "VelopackUpdater.hpp"
#include <Velopack.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include "ClientException.hpp"
#include "ReleaseFeedSource.hpp"
#include "ReleaseOffer.hpp"
#include "UpdaterConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The manager of the installed client, reading @p source.
 * @throws UpdateException when Velopack finds no installed client. */
std::unique_ptr<Velopack::UpdateManager> managerOf(
    std::unique_ptr<ReleaseFeedSource> source) {
  try {
    return std::make_unique<Velopack::UpdateManager>(std::move(source));
  } catch (const std::runtime_error& error) {
    throw UpdateException(LOCATE_STEP, error.what());
  }
}

/** @brief Hands the share Velopack reports to the function @p userData
 * points to. */
void reportProgress(void* userData, std::size_t progress) {
  (*static_cast<std::function<void(int)>*>(userData))(
      static_cast<int>(progress));
}

}  // namespace

VelopackUpdater::VelopackUpdater(std::unique_ptr<ReleaseFeedSource> source)
    : source_(source.get()), manager_(managerOf(std::move(source))) {}

std::string VelopackUpdater::currentVersion() {
  return manager_->GetCurrentVersion();
}

std::optional<ReleaseOffer> VelopackUpdater::newerRelease() {
  std::optional<Velopack::UpdateInfo> update;
  try {
    update = manager_->CheckForUpdates();
  } catch (const std::runtime_error& error) {
    throw UpdateException(CHECK_STEP, error.what());
  }
  if (!update.has_value()) {
    return std::nullopt;
  }
  ReleaseOffer offer{.versionText = update->TargetFullRelease.Version,
                     .notesMarkdown = update->TargetFullRelease.NotesMarkdown};
  const std::scoped_lock lock{mutex_};
  found_ = std::move(update);
  return offer;
}

void VelopackUpdater::download(const std::function<void(int)>& progress) {
  const Velopack::UpdateInfo update = found();
  std::function<void(int)> report = progress;
  try {
    manager_->DownloadUpdates(update, &reportProgress, &report);
  } catch (const std::runtime_error& error) {
    throw UpdateException(DOWNLOAD_STEP, error.what());
  }
}

void VelopackUpdater::applyOnExit() {
  const Velopack::UpdateInfo update = found();
  try {
    manager_->WaitExitThenApplyUpdates(update, SILENT_UPDATER,
                                       RESTART_AFTER_UPDATE);
  } catch (const std::runtime_error& error) {
    throw UpdateException(APPLY_STEP, error.what());
  }
}

void VelopackUpdater::stop() { source_->stop(); }

Velopack::UpdateInfo VelopackUpdater::found() const {
  const std::scoped_lock lock{mutex_};
  if (!found_.has_value()) {
    throw UpdateException(DOWNLOAD_STEP, NO_RELEASE_FOUND);
  }
  return *found_;
}

}  // namespace rtype::client
