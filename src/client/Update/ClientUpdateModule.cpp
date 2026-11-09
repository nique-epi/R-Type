#include "ClientUpdateModule.hpp"
#include <memory>
#include <string>
#include <vector>
#include "IScreen.hpp"
#include "IUpdateNotice.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "UpdateNotice.hpp"
#include "UpdateScreen.hpp"
#include "UpdateScreenIdConstants.hpp"
#include "UpdateScreenParts.hpp"
#include "WhatsNewScreen.hpp"
#include "WhatsNewScreenParts.hpp"

namespace rtype::client {

namespace {

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
