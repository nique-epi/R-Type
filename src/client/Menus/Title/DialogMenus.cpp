#include "DialogMenus.hpp"
#include <string>
#include "ConnectionPhase.hpp"
#include "DialogTools.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "Translation.hpp"

namespace rtype::client {

namespace {

/** @returns Why the session of @p ending ended, as the dialog says it: the
 * server shut down, the host expelled the player, or the server went silent. */
const Translation& lossMessageOf(ConnectionPhase ending) {
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

/** @returns @p text in the language of @p tools. */
std::string inLanguageOf(const DialogTools& tools, const Translation& text) {
  return std::string(text.in(tools.language));
}

}  // namespace

Menu quitConfirmationMenu(const DialogTools& tools) {
  IScreenNavigator& navigator = tools.navigator.get();
  return MenuBuilder(MenuBuilder::Frame::Dialog, &tools.sounds.get())
      .heading(inLanguageOf(tools, CONFIRM_HEADING))
      .message(inLanguageOf(tools, QUIT_QUESTION))
      .cancelButton({.label = inLanguageOf(tools, NO_LABEL)},
                    [&navigator] { navigator.pop(); })
      .button({.label = inLanguageOf(tools, YES_LABEL)},
              [&navigator] { navigator.closeAll(); })
      .build();
}

Menu leaveGameConfirmationMenu(const DialogTools& tools,
                               IServerConnection& connection) {
  IScreenNavigator& navigator = tools.navigator.get();
  return MenuBuilder(MenuBuilder::Frame::Dialog, &tools.sounds.get())
      .heading(inLanguageOf(tools, CONFIRM_HEADING))
      .message(inLanguageOf(tools, QUIT_QUESTION))
      .cancelButton({.label = inLanguageOf(tools, NO_LABEL)},
                    [&navigator] { navigator.pop(); })
      .button({.label = inLanguageOf(tools, YES_LABEL)},
              [&navigator, &connection] {
                connection.cancel();
                navigator.replaceAll(TITLE_SCREEN);
              })
      .build();
}

Menu connectionLostMenu(const DialogTools& tools, ConnectionPhase ending) {
  IScreenNavigator& navigator = tools.navigator.get();
  return MenuBuilder(MenuBuilder::Frame::Dialog, &tools.sounds.get())
      .heading(inLanguageOf(tools, CONNECTION_LOST_HEADING))
      .message(inLanguageOf(tools, lossMessageOf(ending)))
      .button({.label = inLanguageOf(tools, RETRY_LABEL)},
              [&navigator] {
                navigator.pop();
                navigator.push(SERVER_MAP_SCREEN);
                navigator.push(CONNECTION_AGAIN_SCREEN);
              })
      .cancelButton({.label = inLanguageOf(tools, OK_LABEL)},
                    [&navigator] { navigator.pop(); })
      .build();
}

}  // namespace rtype::client
