#include "DialogMenus.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "ScreenConstants.hpp"

namespace rtype::client {

Menu quitConfirmationMenu(IScreenNavigator& navigator, ISoundPlayer& sounds) {
  return MenuBuilder(MenuBuilder::Frame::Dialog, &sounds)
      .heading(CONFIRM_HEADING)
      .message(QUIT_QUESTION)
      .cancelButton({.label = NO_LABEL}, [&navigator] { navigator.pop(); })
      .button({.label = YES_LABEL}, [&navigator] { navigator.closeAll(); })
      .build();
}

}  // namespace rtype::client
