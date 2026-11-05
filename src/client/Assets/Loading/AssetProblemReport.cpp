#include "AssetProblemReport.hpp"
#include <string_view>
#include "AssetLibrary.hpp"
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"
#include "PathText.hpp"

namespace rtype::client {

AssetProblemReport::AssetProblemReport(const AssetLibrary& library)
    : library_(&library) {}

void AssetProblemReport::record(std::string_view assetId,
                                AssetLoadResult result) {
  switch (result) {
    case AssetLoadResult::Loaded:
      return;
    case AssetLoadResult::InvalidId:
      problems_.invalidIds.emplace_back(assetId);
      return;
    case AssetLoadResult::MissingFile:
      problems_.missingFiles.push_back(
          logging::genericText(library_->fileOf(assetId)));
      return;
    case AssetLoadResult::UnreadableFile:
      problems_.unreadableFiles.push_back(
          logging::genericText(library_->fileOf(assetId)));
      return;
  }
}

void AssetProblemReport::throwIfAny() const {
  if (problems_.invalidIds.empty() && problems_.missingFiles.empty() &&
      problems_.unreadableFiles.empty()) {
    return;
  }
  throw AssetLoadingException(logging::genericText(library_->folder()),
                              problems_);
}

}  // namespace rtype::client
