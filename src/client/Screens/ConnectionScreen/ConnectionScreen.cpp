#include "ConnectionScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "ConnectionPhase.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionScreenConstants.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "NetworkException.hpp"
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
    case network::RefusalReason::IncompatibleVersion:
      break;
  }
  return versionRefusalLine(status.serverVersion);
}

}  // namespace

ConnectionScreen::ConnectionScreen(ConnectionScreenParts parts,
                                   std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      connection_(&parts.connection.get()),
      choice_(&parts.choice.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      menu_(buildMenu(parts.sounds.get())) {
  connection_->cancel();
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

Menu ConnectionScreen::buildMenu(ISoundPlayer& sounds) {
  const ServerChoice& choice = *choice_;
  const std::function<void()> connectAction = [this] { connect(); };
  return MenuBuilder(MenuBuilder::Frame::Form, &sounds)
      .heading(CONNECTION_HEADING)
      .textField({.label = ADDRESS_LABEL, .description = ADDRESS_DESCRIPTION},
                 TextField(choice.server.address, ADDRESS_FIELD_SIZE,
                           TextField::Characters::DigitsAndDots),
                 connectAction)
      .textField({.label = PORT_LABEL, .description = PORT_DESCRIPTION},
                 TextField(std::to_string(choice.server.port), PORT_FIELD_SIZE,
                           TextField::Characters::Digits),
                 connectAction)
      .textField({.label = NICKNAME_LABEL, .description = NICKNAME_DESCRIPTION},
                 TextField(choice.nickname, NICKNAME_FIELD_SIZE,
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
  lobbyAsked_ = false;
  try {
    const network::Endpoint server{
        .address = network::ipv4AddressFromText(menu_.fieldText(ADDRESS_ENTRY)),
        .port = network::portFromText(menu_.fieldText(PORT_ENTRY))};
    const std::string_view nickname = menu_.fieldText(NICKNAME_ENTRY);
    if (!network::isValidNickname(nickname)) {
      fieldProblem_ = MISSING_NICKNAME_STATUS;
      return;
    }
    fieldProblem_ = {};
    *choice_ =
        ServerChoice{.server = server, .nickname = std::string(nickname)};
    connection_->connect(server, nickname);
  } catch (const network::InvalidIpv4AddressException&) {
    fieldProblem_ = INVALID_ADDRESS_STATUS;
  } catch (const network::InvalidPortException&) {
    fieldProblem_ = INVALID_PORT_STATUS;
  }
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
  if (!lobbyAsked_ && acceptedFor_ >= ACCEPTED_STATUS_DURATION) {
    lobbyAsked_ = true;
    navigator_->push(ScreenId::Lobby);
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
