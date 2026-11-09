#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include "IUpdateNotice.hpp"
#include "IUpdateRun.hpp"
#include "InstallPlace.hpp"
#include "Logger.hpp"
#include "ReleaseCheck.hpp"
#include "ReleaseNotes.hpp"
#include "UpdateRun.hpp"

namespace rtype::client {

/**
 * @brief What the update module tells the title, and owns to answer it: where
 * the client is installed, the check of the latest release, the update once
 * the panel asks for it, and the notes an update left for this start.
 *
 * Built empty; start() reads the notes and starts the check, stop() stops the
 * check and the update.
 */
class UpdateNotice final : public IUpdateNotice {
 public:
  UpdateNotice();

  [[nodiscard]] bool updateAvailable() const override;
  [[nodiscard]] bool notesWaiting() const override;

  void start();
  void stop();

  /** @returns The update of the release found, started on the first call;
   * call it only once updateAvailable() said so. */
  [[nodiscard]] IUpdateRun& run();
  /** @returns VER <this version> > VER <the release found>. */
  [[nodiscard]] std::string versionLine() const;
  /** @returns The notes waiting; call it only once notesWaiting() said so. */
  [[nodiscard]] ReleaseNotes notes() const;
  /** @brief Forgets the notes, and removes their file. */
  void markNotesShown();

 private:
  void readNotes();

  logging::Logger logger_;
  std::optional<InstallPlace> place_;
  std::optional<ReleaseNotes> notes_;
  std::filesystem::path notesFile_;
  std::unique_ptr<ReleaseCheck> check_;
  std::unique_ptr<UpdateRun> run_;
};

}  // namespace rtype::client
