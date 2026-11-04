#include "AssetProblemRecording.hpp"
#include "AssetLoadAttempt.hpp"
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"
#include "PathText.hpp"

namespace rtype::client {

void recordProblem(AssetProblems& problems, const AssetLoadAttempt& attempt) {
  switch (attempt.result) {
    case AssetLoadResult::Loaded:
      return;
    case AssetLoadResult::InvalidId:
      problems.invalidIds.push_back(attempt.assetId);
      return;
    case AssetLoadResult::MissingFile:
      problems.missingFiles.push_back(engine::genericText(attempt.file));
      return;
    case AssetLoadResult::UnreadableFile:
      problems.unreadableFiles.push_back(engine::genericText(attempt.file));
      return;
  }
}

bool hasProblems(const AssetProblems& problems) {
  return !problems.invalidIds.empty() || !problems.missingFiles.empty() ||
         !problems.unreadableFiles.empty();
}

}  // namespace rtype::client
