#pragma once

#include <memory>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "ClientScreens.hpp"
#include "Endpoint.hpp"
#include "EntityMessageQueue.hpp"
#include "EventListeners.hpp"
#include "FrameLoop.hpp"
#include "GameWindow.hpp"
#include "IAudioService.hpp"
#include "IModule.hpp"
#include "ISettingsStorage.hpp"
#include "InstallContext.hpp"
#include "PlayerSettings.hpp"
#include "ServerSession.hpp"
#include "SubscriptionHandle.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"

namespace rtype::client {

/**
 * @brief The module of the screens of the client and of what they share
 * (ClientScreens), listed last, until modules register screens of their own.
 *
 * install() requires the services of every module listed before it and adds
 * two works. In UPDATE_STAGE, the sky and every screen are updated,
 * then the frame loop stops once no screen is left. In RENDER_STAGE, the sky
 * is drawn, then the screens over it.
 *
 * start() builds the screens, which open on the intro, or on the title when
 * the system asks for less motion, then adds its listener of the window
 * events: every event no module started after it keeps goes to the screen on
 * top. stop() removes that listener; the screens stay until the module is
 * destroyed, after every module stopped, so the music plays on while the
 * connection module leaves the server.
 */
class ClientScreensModule final : public engine::IModule {
 public:
  /**
   * @param server The server the connection screen starts with.
   * @param nickname The nickname the connection screen starts with.
   */
  ClientScreensModule(network::Endpoint server, std::string nickname);

  /** @returns SCREENS_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The modules whose services the screens are built from: the
   * assets, the settings, the sound, the window and the connection. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
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

  network::Endpoint server_;
  std::string nickname_;
  PlayerSettings* settings_{nullptr};
  ISettingsStorage* storage_{nullptr};
  AssetLibrary* assets_{nullptr};
  IAudioService* audio_{nullptr};
  GameWindow* window_{nullptr};
  ServerSession* connection_{nullptr};
  WaitingRoom* waitingRoom_{nullptr};
  EntityMessageQueue* entityMessages_{nullptr};
  FrameLoop* loop_{nullptr};
  EventListeners* listeners_{nullptr};
  std::unique_ptr<ClientScreens> screens_;
  engine::SubscriptionHandle listenerHandle_{};
};

}  // namespace rtype::client
