#include "UpdateRunFixtures.hpp"
#include <filesystem>
#include <string>
#include "FakeReleaseUpdater.hpp"
#include "ReleaseOffer.hpp"
#include "RunFixtures.hpp"
#include "RunTestConstants.hpp"
#include "UpdateConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateRun.hpp"
#include "UpdateRunParts.hpp"

namespace rtype::client::testing {

UpdateRunParts updateOfTheTagNotes(FakeReleaseUpdater& updater,
                                   const std::filesystem::path& folder) {
  return UpdateRunParts{
      .updater = updater,
      .offer = ReleaseOffer{.versionText = std::string{NOTES_VERSION},
                            .notesMarkdown = std::string{TAG_NOTES}},
      .notesFile = folder / RELEASE_NOTES_FILE_NAME};
}

bool reaches(const UpdateRun& run, UpdatePhase phase) {
  return heldWithinTheWait(
      [&run, phase] { return run.progress().phase == phase; });
}

}  // namespace rtype::client::testing
