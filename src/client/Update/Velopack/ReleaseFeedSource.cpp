#include "ReleaseFeedSource.hpp"
#include <Velopack.hpp>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "IReleaseFiles.hpp"
#include "ReleaseDownload.hpp"
#include "UpdateConstants.hpp"
#include "VelopackConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p name names one file of a release: letters, digits and
 * FILE_NAME_PUNCTUATION, never starting with a dot, at most
 * MAXIMUM_FILE_NAME_SIZE long. A name such as "../x" or "a/b" is refused, so
 * a name can never leave the folder of its release. */
bool isFileName(std::string_view name) {
  return !name.empty() && name.size() <= MAXIMUM_FILE_NAME_SIZE &&
         name.front() != FILE_NAME_DOT &&
         std::ranges::all_of(name, [](char character) {
           return std::isalnum(static_cast<unsigned char>(character)) != 0 ||
                  std::ranges::find(FILE_NAME_PUNCTUATION, character) !=
                      FILE_NAME_PUNCTUATION.end();
         });
}

/** @returns Whether @p asset may be downloaded: its file and version are
 * plain names and its size at most MAXIMUM_PACKAGE_BYTES. */
bool isDownloadable(const Velopack::VelopackAsset& asset) {
  return isFileName(asset.FileName) && isFileName(asset.Version) &&
         asset.Size <= MAXIMUM_PACKAGE_BYTES;
}

}  // namespace

ReleaseFeedSource::ReleaseFeedSource(std::unique_ptr<IReleaseFiles> files)
    : files_(std::move(files)), logger_(std::string{UPDATE_LOGGER_NAME}) {}

const std::string ReleaseFeedSource::GetReleaseFeed(
    const std::string releasesName) {
  try {
    return latestFeed(releasesName);
  } catch (const std::exception& error) {
    logger_.warn("the feed could not be read: ", error.what());
  }
  return {};
}

bool ReleaseFeedSource::DownloadReleaseEntry(
    const Velopack::VelopackAsset& asset, const std::string localFilePath,
    Velopack::vpkc_progress_send_t progress) {
  try {
    return saved(asset, localFilePath, progress);
  } catch (const std::exception& error) {
    logger_.warn("the package could not be saved: ", error.what());
  }
  return false;
}

void ReleaseFeedSource::stop() { files_->stop(); }

std::string ReleaseFeedSource::latestFeed(const std::string& releasesName) {
  if (!isFileName(releasesName)) {
    logger_.warn("Velopack asked for a feed of an unexpected name");
    return {};
  }
  const std::optional<std::string> feed = files_->text(
      std::string{LATEST_RELEASE_PATH} + releasesName, MAXIMUM_FEED_BYTES);
  if (!feed.has_value()) {
    logger_.info("the feed of the latest release could not be read");
    return {};
  }
  return *feed;
}

bool ReleaseFeedSource::saved(const Velopack::VelopackAsset& asset,
                              const std::string& localFilePath,
                              const Velopack::vpkc_progress_send_t& progress) {
  if (!isDownloadable(asset)) {
    logger_.warn("the feed lists a package that cannot be downloaded");
    return false;
  }
  return files_->save(ReleaseDownload{
      .path = std::string{RELEASE_DOWNLOAD_PATH} + asset.Version +
              RELEASE_PATH_SEPARATOR + asset.FileName,
      .file = std::filesystem::path{localFilePath},
      .size = asset.Size,
      .progress = [&progress](int percent) {
        progress(static_cast<std::int16_t>(percent));
      }});
}

}  // namespace rtype::client
