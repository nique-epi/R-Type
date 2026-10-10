#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "IScreen.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

/**
 * @brief Loads the assets of a game one file per frame, showing the progress,
 * then gives way to the game.
 *
 * Drawn between two files, it keeps the window responsive however many files
 * there are. Once every file is loaded, it asks the stack to replace every
 * screen with the next one; with nothing to load, that happens at its first
 * frame and it is never seen.
 */
class LoadingScreen : public IScreen {
 public:
  /**
   * @brief Releases the loaded assets the list does not name, except the
   * permanent ones: build it once the screens that used them are destroyed,
   * as ScreenStack::replaceAll() does.
   *
   * @param screens Stack the screen lives in, which must outlive it.
   * @param library Library the assets are loaded into, which must outlive the
   * screen.
   * @param assets Assets the next screen needs.
   * @param nextScreen Builds the screen shown once every asset is loaded.
   */
  LoadingScreen(ScreenStack& screens, AssetLibrary& library,
                const AssetList& assets, ScreenFactory nextScreen);

  /** @brief Ignores every event: nothing can be done while a game loads. */
  void handleEvent(const sf::Event& event) override;

  /**
   * @brief Loads the next file, then asks for the next screen once none is
   * left.
   *
   * @throws AssetLoadingException after the last file, naming every asset that
   * could not be loaded.
   */
  void update() override;

  void draw(sf::RenderTarget& target) const override;

 private:
  void showProgress();

  ScreenStack* screens_;
  AssetLoadingStep loading_;
  ScreenFactory nextScreen_;
  sf::RectangleShape barOutline_;
  sf::RectangleShape barFill_;
};

}  // namespace rtype::client
