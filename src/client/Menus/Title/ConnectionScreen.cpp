#include "ConnectionScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "ConnectionGoal.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionScreenConstants.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionStart.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "EndpointForm.hpp"
#include "EndpointReading.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "LobbyScreenIdConstants.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PlayfieldMapping.hpp"
#include "ProtocolConstants.hpp"
#include "ScreenId.hpp"
#include "ServerChoice.hpp"
#include "TextField.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

namespace {

/** @returns The status line of a refusal for another version: the protocol
 * version of the client, then the one of the server. */
std::string versionRefusalLine(std::uint8_t serverVersion) {
  return INCOMPATIBLE_VERSION_STATUS +
         std::to_string(static_cast<unsigned int>(network::PROTOCOL_VERSION)) +
         SERVER_VERSION_STATUS +
         std::to_string(static_cast<unsigned int>(serverVersion));
}

/** @returns The status line of the refusal @p status reports. */
std::string refusalLine(const ConnectionStatus& status) {
  switch (status.refusal) {
    case network::RefusalReason::GameFull:
      return GAME_FULL_STATUS;
    case network::RefusalReason::GameInProgress:
      return GAME_IN_PROGRESS_STATUS;
    case network::RefusalReason::NicknameTaken:
      return NICKNAME_TAKEN_STATUS;
    case network::RefusalReason::GameNotFound:
      return GAME_NOT_FOUND_STATUS;
    case network::RefusalReason::ServerFull:
      return SERVER_FULL_STATUS;
    case network::RefusalReason::UnknownDifficulty:
      return UNKNOWN_DIFFICULTY_STATUS;
    case network::RefusalReason::LivesOutOfRange:
      return LIVES_OUT_OF_RANGE_STATUS;
    case network::RefusalReason::IncompatibleVersion:
      break;
  }
  return versionRefusalLine(status.serverVersion);
}

/** @returns The status line of a content pack that downloads, as @p status
 * reports it: the share received in whole percents, 0 for a pack of no byte.
 */
std::string loadingContentLine(const ConnectionStatus& status) {
  const std::size_t percent =
      status.contentSize == 0
          ? 0
          : (status.contentReceived * WHOLE_PERCENT) / status.contentSize;
  return LOADING_CONTENT_STATUS + std::to_string(percent) + PERCENT_SIGN;
}

/** @returns The server the fields of a screen started by @p start show: the
 * server the form remembers for ConnectionStart::OnConfirm, the server of the
 * last attempt otherwise, the server the form remembers before any. */
network::Endpoint shownServerOf(const ServerChoice& choice,
                                ConnectionStart start) {
  if (start == ConnectionStart::OnConfirm) {
    return choice.server;
  }
  return choice.lastServer.value_or(choice.server);
}

}  // namespace

ConnectionScreen::ConnectionScreen(ConnectionScreenParts parts,
                                   std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      connection_(&parts.connection.get()),
      choice_(&parts.choice.get()),
      goal_(parts.goal),
      remembersServer_(parts.start == ConnectionStart::OnConfirm),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      menu_(buildMenu(parts.sounds.get(),
                      shownServerOf(parts.choice.get(), parts.start))) {
  connection_->cancel();
  if (parts.start == ConnectionStart::AtOnce) {
    connect();
  }
  repaint(connection_->status());
}

void ConnectionScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, playfieldPointAt_);
}

void ConnectionScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  const ConnectionStatus status = connection_->status();
  followConnection(status, elapsed);
  repaint(status);
}

void ConnectionScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool ConnectionScreen::hidesScreensBelow() const { return true; }

Menu ConnectionScreen::buildMenu(ISoundPlayer& sounds,
                                 const network::Endpoint& shown) {
  const std::function<void()> connectAction = [this] { connect(); };
  MenuBuilder builder(MenuBuilder::Frame::Form, &sounds);
  builder.heading(CONNECTION_HEADING);
  return addEndpointFields(builder, shown, connectAction)
      .textField({.label = NICKNAME_LABEL, .description = NICKNAME_DESCRIPTION},
                 TextField(choice_->nickname, NICKNAME_FIELD_SIZE,
                           TextField::Characters::Printable),
                 connectAction)
      .button({.label = CONNECT_LABEL, .description = CONNECT_DESCRIPTION},
              connectAction)
      .cancelButton({.label = BACK_LABEL}, [this] { leave(); })
      .build();
}

void ConnectionScreen::connect() {
  connection_->cancel();
  acceptedFor_ = engine::Duration::zero();
  goalReached_ = false;
  const EndpointReading reading = readEndpoint(menu_);
  fieldProblem_ = reading.problem;
  if (!reading.endpoint.has_value()) {
    return;
  }
  const network::Endpoint& server = *reading.endpoint;
  const std::string_view nickname = menu_.fieldText(NICKNAME_ENTRY);
  if (!network::isValidNickname(nickname)) {
    fieldProblem_ = MISSING_NICKNAME_STATUS;
    return;
  }
  choice_->lastServer = server;
  if (remembersServer_) {
    choice_->server = server;
  }
  choice_->nickname = std::string(nickname);
  connection_->connect(server, nickname, choice_->game);
}

void ConnectionScreen::leave() {
  connection_->cancel();
  navigator_->pop();
}

void ConnectionScreen::followConnection(const ConnectionStatus& status,
                                        engine::Duration elapsed) {
  if (status.phase != ConnectionPhase::Accepted) {
    acceptedFor_ = engine::Duration::zero();
    return;
  }
  acceptedFor_ += elapsed;
  if (!goalReached_ && acceptedFor_ >= ACCEPTED_STATUS_DURATION) {
    goalReached_ = true;
    if (goal_ == ConnectionGoal::ScreenBelow) {
      navigator_->pop();
    } else {
      navigator_->push(LOBBY_SCREEN);
    }
  }
}

std::string ConnectionScreen::statusLine(const ConnectionStatus& status) const {
  if (!fieldProblem_.empty()) {
    return std::string(fieldProblem_);
  }
  switch (status.phase) {
    case ConnectionPhase::Connecting:
      return CONNECTING_STATUS;
    case ConnectionPhase::Accepted:
      return ACCEPTED_STATUS + std::to_string(status.playerNumber);
    case ConnectionPhase::Refused:
      return refusalLine(status);
    case ConnectionPhase::Unreachable:
      return UNREACHABLE_STATUS;
    case ConnectionPhase::LoadingContent:
      return loadingContentLine(status);
    case ConnectionPhase::ContentDownloadFailed:
      return PACK_DOWNLOAD_FAILED_STATUS;
    case ConnectionPhase::ContentRefused:
      return PACK_REFUSED_STATUS;
    case ConnectionPhase::Idle:
    case ConnectionPhase::Lost:
    case ConnectionPhase::Closed:
      break;
  }
  return {};
}

void ConnectionScreen::repaint(const ConnectionStatus& status) {
  menu_.setStatus(statusLine(status));
  canvas_->clear();
  menu_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
