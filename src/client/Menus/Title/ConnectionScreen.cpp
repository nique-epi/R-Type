#include "ConnectionScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
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
#include "GameRequest.hpp"
#include "GameRuleConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "Language.hpp"
#include "LobbyScreenIdConstants.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PendingJoin.hpp"
#include "PlayfieldMapping.hpp"
#include "ProtocolConstants.hpp"
#include "ScreenId.hpp"
#include "ServerChoice.hpp"
#include "ShownEndpoint.hpp"
#include "TextField.hpp"
#include "TextMark.hpp"
#include "TextMarks.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

namespace {

/** @returns The status line of a refusal for another version: the protocol
 * version of the client, then the one of the server. */
std::string versionRefusalLine(std::uint8_t serverVersion, Language language) {
  const std::string client =
      std::to_string(static_cast<unsigned int>(network::PROTOCOL_VERSION));
  const std::string server =
      std::to_string(static_cast<unsigned int>(serverVersion));
  return withMark(
      withMark(std::string(INCOMPATIBLE_VERSION_STATUS.in(language)),
               TextMark{.mark = CLIENT_VERSION_MARK, .value = client}),
      TextMark{.mark = SERVER_VERSION_MARK, .value = server});
}

/** @returns The status line of a refusal of lives out of bounds: the bounds
 * a server accepts. */
std::string livesRefusalLine(Language language) {
  const std::string lowest = std::to_string(identifiers::LOWEST_LIVES);
  const std::string highest = std::to_string(identifiers::HIGHEST_LIVES);
  return withMark(withMark(std::string(LIVES_REFUSED_STATUS.in(language)),
                           TextMark{.mark = LOWEST_MARK, .value = lowest}),
                  TextMark{.mark = HIGHEST_MARK, .value = highest});
}

/** @returns The status line of the refusal @p status reports, in
 * @p language. */
std::string refusalLine(const ConnectionStatus& status, Language language) {
  switch (status.refusal) {
    case network::RefusalReason::GameFull:
      return std::string(GAME_FULL_STATUS.in(language));
    case network::RefusalReason::GameInProgress:
      return std::string(GAME_IN_PROGRESS_STATUS.in(language));
    case network::RefusalReason::NicknameTaken:
      return std::string(NICKNAME_TAKEN_STATUS.in(language));
    case network::RefusalReason::GameNotFound:
      return std::string(GAME_NOT_FOUND_STATUS.in(language));
    case network::RefusalReason::ServerFull:
      return std::string(SERVER_FULL_STATUS.in(language));
    case network::RefusalReason::UnknownDifficulty:
      return std::string(UNKNOWN_DIFFICULTY_STATUS.in(language));
    case network::RefusalReason::LivesOutOfRange:
      return livesRefusalLine(language);
    case network::RefusalReason::IncompatibleVersion:
      break;
  }
  return versionRefusalLine(status.serverVersion, language);
}

/** @returns The status line of a player the server took, with its
 * number. */
std::string acceptedLine(std::uint8_t playerNumber, Language language) {
  const std::string number =
      std::to_string(static_cast<unsigned int>(playerNumber));
  return withMark(std::string(ACCEPTED_STATUS.in(language)),
                  TextMark{.mark = NUMBER_MARK, .value = number});
}

/** @returns The status line of a content pack that downloads, as @p status
 * reports it: the share received in whole percents, 0 for a pack of no byte.
 */
std::string loadingContentLine(const ConnectionStatus& status,
                               Language language) {
  const std::size_t percent =
      status.contentSize == 0
          ? 0
          : (status.contentReceived * WHOLE_PERCENT) / status.contentSize;
  const std::string share = std::to_string(percent);
  return withMark(std::string(LOADING_CONTENT_STATUS.in(language)),
                  TextMark{.mark = PERCENT_MARK, .value = share});
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

/** @returns Whether an attempt of a screen started by @p start writes the
 * server the form remembers: only ConnectionStart::OnConfirm does. */
bool remembersServer(ConnectionStart start) {
  return start == ConnectionStart::OnConfirm;
}

/** @returns Whether a screen started by @p start shows the ADDRESS and PORT
 * fields: all but ConnectionStart::NicknameOnConfirm do. */
bool asksServer(ConnectionStart start) {
  return start != ConnectionStart::NicknameOnConfirm;
}

/** @returns The server an attempt goes to: the one the ADDRESS and PORT
 * fields of @p menu name when the screen asks for it, the server it shows
 * otherwise, the one the server map wrote. */
EndpointReading serverOf(const Menu& menu, const ServerChoice& choice,
                         ConnectionStart start) {
  if (asksServer(start)) {
    return readEndpoint(menu);
  }
  return EndpointReading{.endpoint = shownServerOf(choice, start),
                         .problem = {}};
}

/** @returns The game a screen started by @p start asks for: the game given at
 * launch for a form that remembers its server, the game of the last attempt
 * otherwise, the game given at launch before any. */
GameRequest requestOf(const ServerChoice& choice, ConnectionStart start) {
  if (remembersServer(start) || !choice.lastRequest.has_value()) {
    return gameRequestFor(choice.game);
  }
  return *choice.lastRequest;
}

/** @brief Moves the last attempt of @p choice to the next server QUICK PLAY
 * kept, when @p status says the server of that attempt is full and the screen
 * was started by @p start as the server map starts it.
 * @returns Whether it moved: the attempt is then started again. */
bool movesToTheNextServer(const ConnectionStatus& status, ServerChoice& choice,
                          ConnectionStart start) {
  if (start != ConnectionStart::NicknameOnConfirm ||
      status.phase != ConnectionPhase::Refused ||
      status.refusal != network::RefusalReason::ServerFull ||
      choice.nextServers.empty()) {
    return false;
  }
  choice.lastServer = choice.nextServers.front();
  choice.nextServers.erase(choice.nextServers.begin());
  return true;
}

/** @brief Starts the attempt @p join on @p connection. */
void startJoin(IServerConnection& connection, const PendingJoin& join) {
  connection.connect(join.server, join.nickname, join.request);
}

}  // namespace

ConnectionScreen::ConnectionScreen(ConnectionScreenParts parts,
                                   std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      connection_(&parts.connection.get()),
      choice_(&parts.choice.get()),
      goal_(parts.goal),
      start_(parts.start),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      ticketWait_(parts.account.get(), parts.tickets.get()),
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
  if (const std::optional<PendingJoin> ready = ticketWait_.advance(elapsed)) {
    startJoin(*connection_, *ready);
  }
  if (movesToTheNextServer(connection_->status(), *choice_, start_)) {
    connect();
  }
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
  const Language language = canvas_->language();
  MenuBuilder builder(MenuBuilder::Frame::Form, &sounds);
  builder.heading(std::string(
      (asksServer(start_) ? CONNECTION_HEADING : JOIN_SERVER_HEADING)
          .in(language)));
  if (asksServer(start_)) {
    addEndpointFields(builder,
                      ShownEndpoint{.endpoint = shown, .language = language},
                      connectAction);
  }
  return builder
      .textField(
          {.label = std::string(NICKNAME_LABEL.in(language)),
           .description = std::string(NICKNAME_DESCRIPTION.in(language))},
          TextField(choice_->nickname, NICKNAME_FIELD_SIZE,
                    TextField::Characters::Printable),
          connectAction)
      .button({.label = std::string(CONNECT_LABEL.in(language)),
               .description = std::string(CONNECT_DESCRIPTION.in(language))},
              connectAction)
      .cancelButton({.label = BACK_LABEL}, [this] { leave(); })
      .build();
}

void ConnectionScreen::connect() {
  connection_->cancel();
  ticketWait_.drop();
  acceptedFor_ = engine::Duration::zero();
  goalReached_ = false;
  const EndpointReading reading = serverOf(menu_, *choice_, start_);
  fieldProblem_ = reading.problem;
  if (!reading.endpoint.has_value()) {
    return;
  }
  const network::Endpoint& server = *reading.endpoint;
  const std::string_view nickname = menu_.fieldText(
      asksServer(start_) ? NICKNAME_ENTRY : NICKNAME_ALONE_ENTRY);
  if (!network::isValidNickname(nickname)) {
    fieldProblem_ = MISSING_NICKNAME_STATUS;
    return;
  }
  const GameRequest request = requestOf(*choice_, start_);
  choice_->lastServer = server;
  choice_->lastRequest = request;
  if (remembersServer(start_)) {
    choice_->server = server;
  }
  choice_->nickname = std::string(nickname);
  if (const std::optional<PendingJoin> now =
          ticketWait_.begin(PendingJoin{.server = server,
                                        .nickname = std::string(nickname),
                                        .request = request})) {
    startJoin(*connection_, *now);
  }
}

void ConnectionScreen::leave() {
  connection_->cancel();
  ticketWait_.drop();
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
  const Language language = canvas_->language();
  if (fieldProblem_.has_value()) {
    return std::string(fieldProblem_->in(language));
  }
  if (ticketWait_.waiting()) {
    return std::string(CONNECTING_STATUS.in(language));
  }
  switch (status.phase) {
    case ConnectionPhase::Connecting:
      return std::string(CONNECTING_STATUS.in(language));
    case ConnectionPhase::Accepted:
      return acceptedLine(status.playerNumber, language);
    case ConnectionPhase::Refused:
      return refusalLine(status, language);
    case ConnectionPhase::Unreachable:
      return std::string(UNREACHABLE_STATUS.in(language));
    case ConnectionPhase::LoadingContent:
      return loadingContentLine(status, language);
    case ConnectionPhase::ContentDownloadFailed:
      return std::string(PACK_DOWNLOAD_FAILED_STATUS.in(language));
    case ConnectionPhase::ContentRefused:
      return std::string(PACK_REFUSED_STATUS.in(language));
    case ConnectionPhase::Idle:
    case ConnectionPhase::Lost:
    case ConnectionPhase::Closed:
    case ConnectionPhase::Expelled:
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
