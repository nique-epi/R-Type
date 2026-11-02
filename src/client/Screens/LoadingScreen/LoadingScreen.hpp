#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "AssetLoadingStep.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "LoadingScreenParts.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Loads the assets of a screen one file per frame, showing the
 * progress, then gives way to that screen.
 *
 * Drawn between two files, it keeps the window responsive however many files
 * there are. Once every file is loaded, it asks the stack to replace every
 * screen with the next one, by its identifier; the factory of that identifier
 * then finds its assets loaded and builds the screen itself. With nothing to
 * load, that happens at its first frame and it is never seen.
 */
class LoadingScreen : public IScreen {
 public:
  /**
   * @brief Releases the loaded assets the list does not name, except the
   * permanent ones: build it once the screens that used them are destroyed,
   * as ScreenStack::replaceAll() does.
   *
   * @param parts The screens it lives among, the library and the assets to
   * load, and the screen shown once every asset is loaded.
   */
  explicit LoadingScreen(const LoadingScreenParts& parts);

  /** @brief Ignores every event: nothing can be done while a game loads. */
  void handleEvent(const sf::Event& event) override;

  /**
   * @brief Loads the next file, then asks for the next screen once none is
   * left.
   *
   * @throws AssetLoadingException after the last file, naming every asset that
   * could not be loaded.
   */
  void update(engine::Duration elapsed) override;

  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  void showProgress();

  IScreenNavigator* navigator_;
  AssetLoadingStep loading_;
  ScreenId nextScreen_;
  bool nextScreenAsked_ = false;
  sf::RectangleShape barOutline_;
  sf::RectangleShape barFill_;
};

}  // namespace rtype::client
