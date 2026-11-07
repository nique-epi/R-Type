#pragma once

#include <memory>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "ChatChannel.hpp"
#include "ClientScreens.hpp"
#include "ConfigurationRegistry.hpp"
#include "Endpoint.hpp"
#include "EntityMessageQueue.hpp"
#include "EntityTypeLooks.hpp"
#include "EventListeners.hpp"
#include "FrameLoop.hpp"
#include "GameWindow.hpp"
#include "IAudioService.hpp"
#include "IEntityTypeTable.hpp"
#include "IGameCatalog.hpp"
#include "IMasterAddress.hpp"
#include "IModule.hpp"
#include "IOptionalDependencies.hpp"
#include "IServerCatalog.hpp"
#include "IServerTicks.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "PlayStart.hpp"
#include "PlayerSettings.hpp"
#include "ScreenRegistry.hpp"
#include "ServerChoice.hpp"
#include "ServerLeaderboard.hpp"
#include "ServerSession.hpp"
#include "SettingValue.hpp"
#include "SettingsService.hpp"
#include "ShipLookExchange.hpp"
#include "SilentAudioService.hpp"
#include "SubscriptionHandle.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The module of the screens of the client and of what they share
 * (ClientScreens), and of the ScreenRegistry the modules listed after it add
 * their screens to.
 *
 * install() provides the registry, still empty, requires the services of every
 * module listed before it, builds the EntityTypeLooks the game reads the entity
 * types of the server from, and adds two works. The sound is the one service it
 * works without: with no sound module before it, the screens get a
 * SilentAudioService the module holds, and start() writes in the journal that
 * the client runs without sound. In UPDATE_STAGE, the sky and
 * every screen are updated, then the frame loop stops once no screen is left.
 * In RENDER_STAGE, the sky is drawn, then the screens over it.
 *
 * install() also declares the `nickname`, `server-address` and `server-port`
 * settings, whose defaults are the server and the nickname given to the
 * constructor. start() reads them once the configuration was read, logs the
 * server, and builds the screens from the registry, which every module filled
 * while it installed, since an Application installs every module before it
 * starts any. They open on the intro, or on the title when the system asks for
 * less motion. It then adds its listener of the window events: every event no
 * module started after it keeps goes to the screen on top. stop() removes that
 * listener; the screens stay until the module is destroyed, after every module
 * stopped, so the music plays on while the connection module leaves the server.
 */
class ClientScreensModule final : public engine::IModule,
                                  public engine::IOptionalDependencies {
 public:
  /**
   * @param launch The server and the nickname the connection screen starts
   * with, unless the configuration sets others, and the game every attempt
   * asks for.
   * @param playStart Where PLAY leads from the title.
   */
  ClientScreensModule(ServerChoice launch, PlayStart playStart);

  /** @returns SCREENS_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The modules whose services the screens are built from: the
   * assets, the settings, the window and the connection. The sound is not
   * among them: the screens run without it. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @returns AUDIO_MODULE_NAME: the sound, used when it is listed and on,
   * and listed before the screens then. */
  [[nodiscard]] std::vector<std::string> optionalDependencies() const override;
  /** @throws MissingServiceException if a module listed before does not
   * provide a service the screens need. */
  void install(engine::InstallContext& context) override;
  /** @throws RenderTextureNotCreatedException when the sky cannot get its
   * render textures. */
  void start() override;
  void stop() override;

 private:
  /** @brief Updates the screens by @p elapsed, then stops the frame loop once
   * no screen is left. */
  void update(engine::Duration elapsed);

  /** @brief Declares the settings the connection screen starts from. */
  void declareSettings(engine::ConfigurationRegistry& configuration);

  /** @returns The server the settings hold, which the connection screen
   * starts with. */
  [[nodiscard]] network::Endpoint configuredServer() const;

  ServerChoice launch_;
  PlayStart playStart_;
  const engine::SettingValue* nicknameSetting_{nullptr};
  const engine::SettingValue* addressSetting_{nullptr};
  const engine::SettingValue* portSetting_{nullptr};
  logging::Logger logger_;
  PlayerSettings* settings_{nullptr};
  SettingsService* settingsService_{nullptr};
  AssetLibrary* assets_{nullptr};
  const IEntityTypeTable* entityTable_{nullptr};
  SilentAudioService silence_;
  IAudioService* audio_{nullptr};
  GameWindow* window_{nullptr};
  ServerSession* connection_{nullptr};
  WaitingRoom* waitingRoom_{nullptr};
  ShipLookExchange* shipLooks_{nullptr};
  EntityMessageQueue* entityMessages_{nullptr};
  ServerLeaderboard* leaderboard_{nullptr};
  ChatChannel* chat_{nullptr};
  std::unique_ptr<EntityTypeLooks> entityTypes_;
  const IServerTicks* serverTicks_{nullptr};
  IServerCatalog* serverCatalog_{nullptr};
  IGameCatalog* gameCatalog_{nullptr};
  const IMasterAddress* masterAddress_{nullptr};
  FrameLoop* loop_{nullptr};
  EventListeners* listeners_{nullptr};
  /** @brief Declared before screens_, which keep a reference to it. */
  ScreenRegistry registry_;
  std::unique_ptr<ClientScreens> screens_;
  engine::SubscriptionHandle listenerHandle_{};
};

}  // namespace rtype::client
