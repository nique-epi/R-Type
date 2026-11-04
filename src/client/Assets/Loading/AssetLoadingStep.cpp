#include "AssetLoadingStep.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetProblemReport.hpp"
#include "PendingAsset.hpp"

namespace rtype::client {

AssetLoadingStep::AssetLoadingStep(AssetLibrary& library,
                                   const AssetList& assets)
    : library_(&library), problems_(library) {
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
  problems_.record(asset.assetId, library_->load(asset.kind, asset.assetId));
  ++processedCount_;
  if (isFinished()) {
    problems_.throwIfAny();
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
