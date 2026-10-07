#pragma once

#include "GameWindow.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

class AssetLibrary;

/** @brief Opens the window and runs the frame loop over the screens, starting
 * with the loading screen of a game. */
class Application {
 public:
  explicit Application(AssetLibrary& assets);

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;
  ~Application() = default;

  /** @brief Runs the frame loop until the window is closed or no screen is
   * left. */
  void run();

 private:
  void handleEvents();
  void render();

  [[nodiscard]] ScreenFactory gameLoadingScreen();
  [[nodiscard]] ScreenFactory gameScreen();
  [[nodiscard]] ScreenFactory optionsScreen();

  AssetLibrary* assets_;
  GameWindow window_;
  ScreenStack screens_;
};

}  // namespace rtype::client
