#include "ClientUpdateModule.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "ExecutablePath.hpp"
#include "FeedConstants.hpp"
#include "IReleaseUpdater.hpp"
#include "IScreen.hpp"
#include "IUpdateNotice.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ReleaseFiles.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "SettingsFolder.hpp"
#include "SignedFeedSource.hpp"
#include "UpdateConstants.hpp"
#include "UpdateNotice.hpp"
#include "UpdateScreen.hpp"
#include "UpdateScreenIdConstants.hpp"
#include "UpdateScreenParts.hpp"
#include "VelopackUpdater.hpp"
#include "WhatsNewScreen.hpp"
#include "WhatsNewScreenParts.hpp"

namespace rtype::client {

namespace {

/** @returns The updater of the installed client, reading the signed feeds of
 * the releases of the game.
 * @throws UpdateException when the client was not installed by Velopack. */
std::unique_ptr<IReleaseUpdater> installedClientUpdater() {
  return std::make_unique<VelopackUpdater>(std::make_unique<SignedFeedSource>(
      std::make_unique<ReleaseFiles>(), UPDATE_PUBLIC_KEY));
}

/** @returns Where the notes of an update are kept: the settings folder, or
 * the folder of the executable when the system names none. */
std::filesystem::path notesFile() {
  return settingsFolder().value_or(engine::executableFolder()) /
         RELEASE_NOTES_FILE_NAME;
}

std::unique_ptr<IScreen> buildUpdateScreen(const ScreenContext& context,
                                           UpdateNotice& notice) {
  return std::make_unique<UpdateScreen>(
      UpdateScreenParts{.navigator = context.navigator.get(),
                        .run = notice.run(),
                        .versionLine = notice.versionLine()},
      interfaceCanvasOf(context));
}

std::unique_ptr<IScreen> buildWhatsNewScreen(const ScreenContext& context,
                                             UpdateNotice& notice) {
  return std::make_unique<WhatsNewScreen>(
      WhatsNewScreenParts{.navigator = context.navigator.get(),
                          .notes = notice.notes(),
                          .onClose = [&notice] { notice.markNotesShown(); }},
      interfaceCanvasOf(context));
}

}  // namespace

ClientUpdateModule::ClientUpdateModule()
    : notice_(installedClientUpdater, notesFile()) {}

std::string ClientUpdateModule::name() const {
  return std::string{UPDATE_MODULE_NAME};
}

std::vector<std::string> ClientUpdateModule::dependencies() const {
  return {std::string{SCREENS_MODULE_NAME}};
}

void ClientUpdateModule::install(engine::InstallContext& context) {
  auto& registry = context.require<ScreenRegistry>();
  context.provide<IUpdateNotice>(notice_);
  registry.add(UPDATE_SCREEN, [this](const ScreenContext& screens) {
    return buildUpdateScreen(screens, notice_);
  });
  registry.add(WHATS_NEW_SCREEN, [this](const ScreenContext& screens) {
    return buildWhatsNewScreen(screens, notice_);
  });
}

void ClientUpdateModule::start() { notice_.start(); }

void ClientUpdateModule::stop() { notice_.stop(); }

}  // namespace rtype::client
