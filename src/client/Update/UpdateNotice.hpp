#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include "IReleaseUpdater.hpp"
#include "IUpdateNotice.hpp"
#include "IUpdateRun.hpp"
#include "Logger.hpp"
#include "ReleaseNotes.hpp"
#include "UpdateCheck.hpp"
#include "UpdateRun.hpp"

namespace rtype::client {

/**
 * @brief What the update module tells the title, and owns to answer it: the
 * updater of the installed client, the check of the latest release, the
 * update once the panel asks for it, and the notes an update left for this
 * start.
 *
 * Built empty; start() makes the updater, reads the notes and starts the
 * check; stop() ends the check and the update.
 */
class UpdateNotice final : public IUpdateNotice {
 public:
  /** @param makeUpdater Makes the updater of the installed client, or throws
   * UpdateException when the client was not installed by Velopack.
   * @param notesFile Where the notes of an update are kept. */
  UpdateNotice(std::function<std::unique_ptr<IReleaseUpdater>()> makeUpdater,
               std::filesystem::path notesFile);
  ~UpdateNotice() override;
  UpdateNotice(const UpdateNotice&) = delete;
  UpdateNotice& operator=(const UpdateNotice&) = delete;
  UpdateNotice(UpdateNotice&&) = delete;
  UpdateNotice& operator=(UpdateNotice&&) = delete;

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

  std::function<std::unique_ptr<IReleaseUpdater>()> makeUpdater_;
  std::filesystem::path notesFile_;
  logging::Logger logger_;
  std::unique_ptr<IReleaseUpdater> updater_;
  std::string currentVersion_;
  std::optional<ReleaseNotes> notes_;
  std::unique_ptr<UpdateCheck> check_;
  std::unique_ptr<UpdateRun> run_;
};

}  // namespace rtype::client
