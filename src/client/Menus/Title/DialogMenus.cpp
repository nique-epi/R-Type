#include "DialogMenus.hpp"
#include "ConnectionPhase.hpp"
#include "DialogMaking.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

namespace {

/** @returns Why the session of @p ending ended, as the dialog says it: the
 * server shut down, the host expelled the player, or the server went silent. */
const char* lossMessageOf(ConnectionPhase ending) {
  switch (ending) {
    case ConnectionPhase::Closed:
      return CLOSED_SERVER_MESSAGE;
    case ConnectionPhase::Expelled:
      return EXPELLED_MESSAGE;
    case ConnectionPhase::Idle:
    case ConnectionPhase::Connecting:
    case ConnectionPhase::Accepted:
    case ConnectionPhase::Refused:
    case ConnectionPhase::Unreachable:
    case ConnectionPhase::Lost:
    case ConnectionPhase::LoadingContent:
    case ConnectionPhase::ContentDownloadFailed:
    case ConnectionPhase::ContentRefused:
      break;
  }
  return SILENT_SERVER_MESSAGE;
}

}  // namespace

Menu quitConfirmationMenu(const DialogMaking& making) {
  IScreenNavigator& navigator = making.navigator.get();
  return MenuBuilder(MenuBuilder::Frame::Dialog, &making.sounds.get(),
                     making.metrics)
      .heading(CONFIRM_HEADING)
      .message(QUIT_QUESTION)
      .cancelButton({.label = NO_LABEL}, [&navigator] { navigator.pop(); })
      .button({.label = YES_LABEL}, [&navigator] { navigator.closeAll(); })
      .build();
}

Menu leaveGameConfirmationMenu(const DialogMaking& making,
                               IServerConnection& connection) {
  IScreenNavigator& navigator = making.navigator.get();
  return MenuBuilder(MenuBuilder::Frame::Dialog, &making.sounds.get(),
                     making.metrics)
      .heading(CONFIRM_HEADING)
      .message(QUIT_QUESTION)
      .cancelButton({.label = NO_LABEL}, [&navigator] { navigator.pop(); })
      .button({.label = YES_LABEL},
              [&navigator, &connection] {
                connection.cancel();
                navigator.replaceAll(TITLE_SCREEN);
              })
      .build();
}

Menu connectionLostMenu(const DialogMaking& making, ConnectionPhase ending) {
  IScreenNavigator& navigator = making.navigator.get();
  return MenuBuilder(MenuBuilder::Frame::Dialog, &making.sounds.get(),
                     making.metrics)
      .heading(CONNECTION_LOST_HEADING)
      .message(lossMessageOf(ending))
      .button({.label = RETRY_LABEL},
              [&navigator] {
                navigator.pop();
                navigator.push(SERVER_MAP_SCREEN);
                navigator.push(CONNECTION_AGAIN_SCREEN);
              })
      .cancelButton({.label = OK_LABEL}, [&navigator] { navigator.pop(); })
      .build();
}

}  // namespace rtype::client
