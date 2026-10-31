#include "DialogMenus.hpp"
#include "ConnectionPhase.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

Menu quitConfirmationMenu(IScreenNavigator& navigator, ISoundPlayer& sounds) {
  return MenuBuilder(MenuBuilder::Frame::Dialog, &sounds)
      .heading(CONFIRM_HEADING)
      .message(QUIT_QUESTION)
      .cancelButton({.label = NO_LABEL}, [&navigator] { navigator.pop(); })
      .button({.label = YES_LABEL}, [&navigator] { navigator.closeAll(); })
      .build();
}

Menu connectionLostMenu(IScreenNavigator& navigator, ISoundPlayer& sounds,
                        ConnectionPhase ending) {
  return MenuBuilder(MenuBuilder::Frame::Dialog, &sounds)
      .heading(CONNECTION_LOST_HEADING)
      .message(ending == ConnectionPhase::Closed ? CLOSED_SERVER_MESSAGE
                                                 : SILENT_SERVER_MESSAGE)
      .button({.label = RETRY_LABEL},
              [&navigator] {
                navigator.pop();
                navigator.push(ScreenId::Connection);
              })
      .cancelButton({.label = OK_LABEL}, [&navigator] { navigator.pop(); })
      .build();
}

}  // namespace rtype::client
