#include "ClientScreensModule.hpp"
#include <SFML/Window/Event.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "AssetLibrary.hpp"
#include "ClientConstants.hpp"
#include "ClientScreens.hpp"
#include "Endpoint.hpp"
#include "EventListeners.hpp"
#include "EventUse.hpp"
#include "FrameLoop.hpp"
#include "GameWindow.hpp"
#include "IAudioService.hpp"
#include "ISettingsStorage.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PlayerSettings.hpp"
#include "ScreenRegistry.hpp"
#include "ScreenServices.hpp"
#include "ServerConnection.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientScreensModule::ClientScreensModule(network::Endpoint server,
                                         std::string nickname)
    : server_(std::move(server)), nickname_(std::move(nickname)) {}

std::string ClientScreensModule::name() const {
  return std::string{SCREENS_MODULE_NAME};
}

std::vector<std::string> ClientScreensModule::dependencies() const {
  return {std::string{ASSETS_MODULE_NAME},
          std::string{SETTINGS_MODULE_NAME},
          std::string{AUDIO_MODULE_NAME},
          std::string{WINDOW_MODULE_NAME},
          std::string{CONNECTION_MODULE_NAME},
          std::string{SCREEN_REGISTRY_MODULE_NAME},
          std::string{MENUS_MODULE_NAME},
          std::string{LOBBY_MODULE_NAME},
          std::string{GAME_MODULE_NAME}};
}

void ClientScreensModule::install(engine::InstallContext& context) {
  settings_ = &context.require<PlayerSettings>();
  storage_ = &context.require<ISettingsStorage>();
  assets_ = &context.require<AssetLibrary>();
  audio_ = &context.require<IAudioService>();
  window_ = &context.require<GameWindow>();
  connection_ = &context.require<ServerConnection>();
  loop_ = &context.require<FrameLoop>();
  listeners_ = &context.require<EventListeners>();
  registry_ = &context.require<ScreenRegistry>();
  context.addWork(UPDATE_STAGE,
                  [this](engine::Duration elapsed) { update(elapsed); });
  context.addWork(RENDER_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    screens_->draw(window_->renderTarget());
                  });
}

void ClientScreensModule::start() {
  screens_ =
      std::make_unique<ClientScreens>(ScreenServices{.settings = *settings_,
                                                     .storage = *storage_,
                                                     .assets = *assets_,
                                                     .audio = *audio_,
                                                     .window = *window_,
                                                     .connection = *connection_,
                                                     .server = server_,
                                                     .nickname = nickname_},
                                      *registry_);
  listenerHandle_ = listeners_->add([this](const sf::Event& event) {
    screens_->handleEvent(event);
    return EventUse::Kept;
  });
}

void ClientScreensModule::stop() { listeners_->remove(listenerHandle_); }

void ClientScreensModule::update(engine::Duration elapsed) {
  screens_->update(elapsed);
  if (screens_->isEmpty()) {
    loop_->stop();
  }
}

}  // namespace rtype::client
