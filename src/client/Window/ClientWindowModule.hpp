#pragma once

#include <string>
#include <vector>
#include "EventListeners.hpp"
#include "FrameLoop.hpp"
#include "GameWindow.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "PlayerSettings.hpp"

namespace rtype::client {

/**
 * @brief The module of the window of the client: the GameWindow, the
 * listeners of its events, and the frame loop main() runs, which it provides
 * to the modules installed after it with the window and the listeners.
 *
 * install() provides them, the window still closed, and adds three works. In
 * EVENT_STAGE, each event of the window goes to the listeners, the last added
 * first, then the loop stops once the window was closed. In RENDER_STAGE, the
 * window is cleared before the modules listed after it draw. In
 * DISPLAY_STAGE, the frame they drew is shown.
 *
 * start() opens the window at the size and in the display mode the settings
 * hold, read by the settings module started before it; stop() closes it.
 */
class ClientWindowModule final : public engine::IModule {
 public:
  /** @param loop Run by main(); it must outlive the module. */
  explicit ClientWindowModule(FrameLoop& loop);

  /** @returns WINDOW_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns SETTINGS_MODULE_NAME, whose PlayerSettings give the size of the
   * window. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no PlayerSettings is provided. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  /** @brief Hands each event of the window to the listeners, then stops the
   * loop once the window was closed. */
  void handleEvents();

  FrameLoop* loop_;
  const PlayerSettings* settings_{nullptr};
  GameWindow window_;
  EventListeners listeners_;
};

}  // namespace rtype::client
