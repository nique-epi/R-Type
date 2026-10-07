#pragma once

#include "GameWindow.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief The client once its interface assets are loaded: opens the window
 * and runs the frame loop over the screens.
 *
 * Each frame, the window events go to the screen on top, every screen is
 * updated, then every screen is drawn from the bottom up. The changes the
 * screens ask for are applied after each event and after the update, so the
 * next event already reaches the new screen on top.
 *
 * The loop never waits on the network: reception is meant to run on its own
 * thread and hand its messages over through a queue a screen drains each
 * frame.
 *
 * The client starts by entering a game: the loading screen loads gameAssets(),
 * then gives way to the game screen.
 */
class Application {
 public:
  /** @param assets Library holding the interface assets, loaded and
   * permanent; it must outlive the application. */
  explicit Application(AssetLibrary& assets);

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;
  ~Application() = default;

  /**
   * @brief Runs the frame loop until the window is closed or no screen is
   * left.
   *
   * @throws AssetLoadingException when an asset of the game cannot be loaded.
   */
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
