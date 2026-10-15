#pragma once

#include <memory>
#include "AssetList.hpp"
#include "GameWindow.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenId.hpp"
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
 * The background and every screen move by the real time elapsed since the
 * previous frame, so their speed does not depend on the frame rate. It is drawn
 * under every screen for now, the loading screen and the options included.
 *
 * The loop never waits on the network: reception is meant to run on its own
 * thread and hand its messages over through a queue a screen drains each
 * frame.
 *
 * Screens are asked for by identifier; createScreen() is the one place that
 * builds them, with one case per ScreenId. The client starts on the title
 * screen.
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

  /** @returns The real time since the previous frame, which the background
   * and every screen move by. */
  [[nodiscard]] engine::Duration measureFrame();

  void render();

  /** @brief Builds the screen of an identifier: the list of every screen of
   * the client. */
  [[nodiscard]] std::unique_ptr<IScreen> createScreen(ScreenId screen);

  /** @brief The loading screen of titleAssets() while some are not loaded,
   * the title screen once they all are; its squadron is still when the system
   * asks for less motion. */
  [[nodiscard]] std::unique_ptr<IScreen> titleScreen();

  /** @brief The loading screen of gameAssets() while some are not loaded, the
   * game screen once they all are. */
  [[nodiscard]] std::unique_ptr<IScreen> gameScreen();

  /**
   * @brief The loading screen of a list while some of its assets are not
   * loaded; nothing once they all are, after releasing the loaded assets the
   * list does not name, as the loading screen does.
   *
   * @param screen Screen the loading screen gives way to.
   */
  [[nodiscard]] std::unique_ptr<IScreen> loadingScreenUnlessLoaded(
      const AssetList& assets, ScreenId screen);

  /** @brief A new picture for a menu, written with the fonts of the asset
   * library. */
  [[nodiscard]] std::unique_ptr<IInterfaceCanvas> interfaceCanvas() const;

  /** @brief Gives the point of the playfield under a pixel of the window. */
  [[nodiscard]] PlayfieldMapping playfieldMapping() const;

  AssetLibrary* assets_;
  GameWindow window_;
  ScrollingBackground background_;
  engine::SystemClock clock_;
  engine::Duration previousFrameTime_{};
  ScreenStack screens_;
};

}  // namespace rtype::client
