#pragma once

#include <filesystem>
#include "FakeReleaseUpdater.hpp"
#include "UpdatePhase.hpp"
#include "UpdateRun.hpp"
#include "UpdateRunParts.hpp"

namespace rtype::client::testing {

/** @returns The parts of an update of release NOTES_VERSION, its notes
 * TAG_NOTES kept in a notes file of @p folder. */
[[nodiscard]] UpdateRunParts updateOfTheTagNotes(
    FakeReleaseUpdater& updater, const std::filesystem::path& folder);

/** @returns Whether @p run reached @p phase within the wait. */
[[nodiscard]] bool reaches(const UpdateRun& run, UpdatePhase phase);

}  // namespace rtype::client::testing
