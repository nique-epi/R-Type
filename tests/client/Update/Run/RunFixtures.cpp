#include "RunFixtures.hpp"
#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>
#include "FakeReleaseUpdater.hpp"
#include "ReleaseOffer.hpp"
#include "RunTestConstants.hpp"
#include "UpdateConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateRun.hpp"
#include "UpdateRunParts.hpp"
#include "UpdateTestConstants.hpp"

namespace rtype::client::testing {

bool heldWithinTheWait(const std::function<bool()>& condition) {
  const auto end = std::chrono::steady_clock::now() + THREAD_WAIT;
  while (!condition()) {
    if (std::chrono::steady_clock::now() >= end) {
      return false;
    }
    std::this_thread::sleep_for(THREAD_POLL);
  }
  return true;
}

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
