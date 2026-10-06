#include "AssetLoadingStep.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "AssetIds.hpp"
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"

namespace rtype::client {

namespace {

template <typename Pending>
void appendPending(std::vector<Pending>& pending, AssetKind kind,
                   const std::vector<std::string>& assetIds) {
  for (const std::string& assetId : assetIds) {
    pending.push_back(Pending{.kind = kind, .assetId = assetId});
  }
}

bool hasProblems(const AssetProblems& problems) {
  return !problems.invalidIds.empty() || !problems.missingFiles.empty() ||
         !problems.unreadableFiles.empty();
}

}  // namespace

AssetLoadingStep::AssetLoadingStep(AssetLibrary& library,
                                   const AssetList& nextScreenAssets)
    : library_(&library) {
  library_->releaseAllExcept(nextScreenAssets);
  appendPending(pending_, AssetKind::Texture, nextScreenAssets.textures);
  appendPending(pending_, AssetKind::Sound, nextScreenAssets.sounds);
  appendPending(pending_, AssetKind::Font, nextScreenAssets.fonts);
  appendPending(pending_, AssetKind::Music, nextScreenAssets.music);
}

void AssetLoadingStep::loadNext() {
  if (isFinished()) {
    return;
  }
  const PendingAsset& asset = pending_[processedCount_];
  record(asset, library_->load(asset.kind, asset.assetId));
  ++processedCount_;
  if (isFinished() && hasProblems(problems_)) {
    throw AssetLoadingException(genericText(library_->folder()), problems_);
  }
}

bool AssetLoadingStep::isFinished() const {
  return processedCount_ == pending_.size();
}

std::size_t AssetLoadingStep::processedCount() const { return processedCount_; }

std::size_t AssetLoadingStep::assetCount() const { return pending_.size(); }

void AssetLoadingStep::record(const PendingAsset& asset,
                              AssetLoadResult result) {
  switch (result) {
    case AssetLoadResult::Loaded:
      return;
    case AssetLoadResult::InvalidId:
      problems_.invalidIds.push_back(asset.assetId);
      return;
    case AssetLoadResult::MissingFile:
      problems_.missingFiles.push_back(
          genericText(library_->fileOf(asset.assetId)));
      return;
    case AssetLoadResult::UnreadableFile:
      problems_.unreadableFiles.push_back(
          genericText(library_->fileOf(asset.assetId)));
      return;
  }
}

}  // namespace rtype::client
