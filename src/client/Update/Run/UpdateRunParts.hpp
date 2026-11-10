#pragma once

#include <filesystem>
#include <functional>
#include "IReleaseUpdater.hpp"
#include "ReleaseOffer.hpp"

namespace rtype::client {

/**
 * @brief What an update is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct UpdateRunParts {
  /** @brief What downloads and applies it, which must outlive it. */
  std::reference_wrapper<IReleaseUpdater> updater;
  /** @brief The release installed. */
  ReleaseOffer offer;
  /** @brief Where its notes are kept for the next start. */
  std::filesystem::path notesFile;
};

}  // namespace rtype::client
