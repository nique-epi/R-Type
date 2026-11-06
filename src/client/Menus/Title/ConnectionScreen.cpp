#include "ConnectionScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cctype>
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
#include "EndpointText.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "LobbyScreenIdConstants.hpp"
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
    case network::RefusalReason::GameNotFound:
      return GAME_NOT_FOUND_STATUS;
    case network::RefusalReason::ServerFull:
      return SERVER_FULL_STATUS;
    case network::RefusalReason::IncompatibleVersion:
      break;
  }
  return versionRefusalLine(status.serverVersion);
}

/**
 * @returns @p name in capitals, cut to @p room characters, its last one
 * CUT_NAME_MARK, when it is longer; @p room is at least 2.
 */
std::string shownSheetName(std::string_view name, std::size_t room) {
  std::string shown;
  for (const char character : name) {
    shown.push_back(
        static_cast<char>(std::toupper(static_cast<unsigned char>(character))));
  }
  if (shown.size() > room) {
    shown.resize(room - 1);
    shown.push_back(CUT_NAME_MARK);
  }
  return shown;
}

/** @returns The status line of an acceptance whose table names sheets this
 * client lacks, as @p status reports it, within STATUS_LINE_CHARACTERS. */
std::string missingSheetLine(const ConnectionStatus& status) {
  const std::string others = status.otherMissingSheets == 0
                                 ? std::string{}
                                 : OTHER_MISSING_SHEETS_STATUS +
                                       std::to_string(static_cast<unsigned int>(
                                           status.otherMissingSheets));
  const std::size_t room = STATUS_LINE_CHARACTERS -
                           std::string_view{MISSING_SHEET_STATUS}.size() -
                           others.size();
  return MISSING_SHEET_STATUS +
         shownSheetName(status.missingSheet.value_or(std::string{}), room) +
         others;
}

}  // namespace

ConnectionScreen::ConnectionScreen(ConnectionScreenParts parts,
                                   std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      connection_(&parts.connection.get()),
      choice_(&parts.choice.get()),
      goal_(parts.goal),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      menu_(buildMenu(parts.sounds.get())) {
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
  goalReached_ = false;
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
    choice_->server = server;
    choice_->nickname = std::string(nickname);
    connection_->connect(server, nickname, choice_->game);
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
    case ConnectionPhase::MissingEntitySheet:
      return missingSheetLine(status);
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
