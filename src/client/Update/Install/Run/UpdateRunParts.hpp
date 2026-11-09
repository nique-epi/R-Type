#pragma once

#include <filesystem>
#include "InstallPlace.hpp"
#include "ReleaseOffer.hpp"

namespace rtype::client {

/** @brief What an update is built from: the release found, where the client
 * is installed, where the package is written, and the file the notes go to. */
struct UpdateRunParts {
  ReleaseOffer offer;
  InstallPlace place;
  std::filesystem::path downloadFolder;
  std::filesystem::path notesFile;
};

}  // namespace rtype::client
