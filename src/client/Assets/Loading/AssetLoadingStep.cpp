#include "AssetLoadingStep.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadAttempt.hpp"
#include "AssetProblemRecording.hpp"
#include "ClientException.hpp"
#include "PathText.hpp"
#include "PendingAsset.hpp"

namespace rtype::client {

AssetLoadingStep::AssetLoadingStep(AssetLibrary& library,
                                   const AssetList& assets)
    : library_(&library) {
  library_->releaseAllExcept(assets);
  queueMissing(AssetKind::Texture, assets.textures);
  queueMissing(AssetKind::Font, assets.fonts);
  queueMissing(AssetKind::Music, assets.music);
}

void AssetLoadingStep::loadNext() {
  if (isFinished()) {
    return;
  }
  const PendingAsset& asset = pending_[processedCount_];
  recordProblem(
      problems_,
      AssetLoadAttempt{.assetId = asset.assetId,
                       .file = library_->fileOf(asset.assetId),
                       .result = library_->load(asset.kind, asset.assetId)});
  ++processedCount_;
  if (isFinished() && hasProblems(problems_)) {
    throw AssetLoadingException(engine::genericText(library_->folder()),
                                problems_);
  }
}

bool AssetLoadingStep::isFinished() const {
  return processedCount_ == pending_.size();
}

std::size_t AssetLoadingStep::processedCount() const { return processedCount_; }

std::size_t AssetLoadingStep::assetCount() const { return pending_.size(); }

void AssetLoadingStep::queueMissing(AssetKind kind,
                                    const std::vector<std::string>& assetIds) {
  for (const std::string& assetId : assetIds) {
    if (!library_->isLoaded(kind, assetId)) {
      pending_.push_back(PendingAsset{.kind = kind, .assetId = assetId});
    }
  }
}

}  // namespace rtype::client
