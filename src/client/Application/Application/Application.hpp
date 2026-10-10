#pragma once

#include "GameWindow.hpp"
#include "ScreenStack.hpp"
#include "ScrollingBackground.hpp"
#include "SystemClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief The client once its interface assets are loaded: opens the window
 * and runs the frame loop over the screens.
 *
 * Each frame, the window events go to the screen on top, the scrolling
 * background and every screen are updated, then the background is drawn and
 * every screen over it, from the bottom up. The changes the screens ask for are
 * applied after each event and after the update, so the next event already
 * reaches the new screen on top.
 *
 * The background moves by the real time elapsed since the previous frame, so
 * its speed does not depend on the frame rate. It is drawn under every screen
 * for now, the loading screen and the options included.
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
  /**
   * @param assets Library holding the interface assets, loaded and permanent;
   * it must outlive the application.
   * @throws RenderTextureNotCreatedException when the background cannot get
   * its render textures.
   */
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
   * @throws RenderTextureNotCreatedException when the background cannot be
   * enlarged to the size of the window.
   */
  void run();

 private:
  void handleEvents();

  /** @brief Moves the background by the time since the previous frame. */
  void advanceBackground();

  void render();

  [[nodiscard]] ScreenFactory gameLoadingScreen();
  [[nodiscard]] ScreenFactory gameScreen();
  [[nodiscard]] ScreenFactory optionsScreen();

  AssetLibrary* assets_;
  GameWindow window_;
  ScrollingBackground background_;
  engine::SystemClock clock_;
  engine::Duration previousFrameTime_{};
  ScreenStack screens_;
};

}  // namespace rtype::client
