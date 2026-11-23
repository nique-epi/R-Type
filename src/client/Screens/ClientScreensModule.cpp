#include "ClientScreensModule.hpp"
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AnnouncedStart.hpp"
#include "AssetLibrary.hpp"
#include "AudioServiceChoice.hpp"
#include "ChatChannel.hpp"
#include "ClientConstants.hpp"
#include "ClientScreens.hpp"
#include "ConfigurationRegistry.hpp"
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "EntityMessageQueue.hpp"
#include "EntityTypeLooks.hpp"
#include "EventListeners.hpp"
#include "EventUse.hpp"
#include "FrameLoop.hpp"
#include "GameWindow.hpp"
#include "IAccountSession.hpp"
#include "IDeviceLinkFlow.hpp"
#include "IEntityTypeTable.hpp"
#include "IGameCatalog.hpp"
#include "IMapBallot.hpp"
#include "IServerCatalog.hpp"
#include "IServerTicks.hpp"
#include "ISessionPack.hpp"
#include "ITicketLink.hpp"
#include "IWallet.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "Logger.hpp"
#include "MasterLeaderboard.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PlayStart.hpp"
#include "PlayerSettings.hpp"
#include "ProtocolConstants.hpp"
#include "ScreenServices.hpp"
#include "ServerChoice.hpp"
#include "ServerLeaderboard.hpp"
#include "ServerSession.hpp"
#include "SettingsService.hpp"
#include "ShipLookExchange.hpp"
#include "TextSettingDefinition.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

namespace {

/** @brief Writes to the journal, at the debug level, each key, button and
 * focus event the screens receive, with the code and the scancode of a key:
 * a press whose release never comes, or comes with another code, shows there
 * (`--log-level debug`). */
void logInputEvent(const logging::Logger& logger, const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    logger.debug("key pressed: code ", static_cast<int>(pressed->code),
                 " scancode ", static_cast<int>(pressed->scancode));
  } else if (const auto* released = event.getIf<sf::Event::KeyReleased>()) {
    logger.debug("key released: code ", static_cast<int>(released->code),
                 " scancode ", static_cast<int>(released->scancode));
  } else if (const auto* clicked =
                 event.getIf<sf::Event::MouseButtonPressed>()) {
    logger.debug("button pressed: ", static_cast<int>(clicked->button), " at ",
                 clicked->position.x, ", ", clicked->position.y);
  } else if (const auto* unclicked =
                 event.getIf<sf::Event::MouseButtonReleased>()) {
    logger.debug("button released: ", static_cast<int>(unclicked->button),
                 " at ", unclicked->position.x, ", ", unclicked->position.y);
  } else if (event.is<sf::Event::FocusLost>()) {
    logger.debug("focus lost");
  } else if (event.is<sf::Event::FocusGained>()) {
    logger.debug("focus gained");
  }
}

/**
 * @throws engine::CommandRefusedException when @p nickname is not a nickname
 * the server accepts.
 */
void requireNickname(std::string_view nickname) {
  if (!network::isValidNickname(nickname)) {
    throw engine::CommandRefusedException(
        "'" + std::string(nickname) + "' is not a nickname of " +
        std::to_string(network::MINIMUM_NICKNAME_SIZE) + " to " +
        std::to_string(network::MAXIMUM_NICKNAME_SIZE) + " bytes of UTF-8");
  }
}

/**
 * @throws engine::CommandRefusedException when @p address is not an IPv4
 * address written as four numbers.
 */
void requireIpv4Address(std::string_view address) {
  try {
    static_cast<void>(network::ipv4AddressFromText(address));
  } catch (const network::NetworkException& error) {
    throw engine::CommandRefusedException(error.what());
  }
}

}  // namespace

ClientScreensModule::ClientScreensModule(ServerChoice launch,
                                         PlayStart playStart)
    : launch_(std::move(launch)),
      playStart_(playStart),
      logger_(std::string{CLIENT_LOGGER_NAME}) {}

std::string ClientScreensModule::name() const {
  return std::string{SCREENS_MODULE_NAME};
}

std::vector<std::string> ClientScreensModule::dependencies() const {
  return {std::string{ASSETS_MODULE_NAME}, std::string{SETTINGS_MODULE_NAME},
          std::string{WINDOW_MODULE_NAME}, std::string{CONNECTION_MODULE_NAME},
          std::string{ACCOUNT_MODULE_NAME}};
}

std::vector<std::string> ClientScreensModule::optionalDependencies() const {
  return {std::string{AUDIO_MODULE_NAME}};
}

void ClientScreensModule::install(engine::InstallContext& context) {
  context.provide(registry_);
  settings_ = &context.require<PlayerSettings>();
  settingsService_ = &context.require<SettingsService>();
  assets_ = &context.require<AssetLibrary>();
  audio_ = &chooseAudioService(context, silence_);
  window_ = &context.require<GameWindow>();
  connection_ = &context.require<ServerSession>();
  waitingRoom_ = &context.require<WaitingRoom>();
  announcedStart_ = &context.require<AnnouncedStart>();
  shipLooks_ = &context.require<ShipLookExchange>();
  mapBallot_ = &context.require<IMapBallot>();
  entityMessages_ = &context.require<EntityMessageQueue>();
  leaderboard_ = &context.require<ServerLeaderboard>();
  globalLeaderboard_ = &context.require<MasterLeaderboard>();
  chat_ = &context.require<ChatChannel>();
  entityTable_ = &context.require<IEntityTypeTable>();
  sessionPack_ = &context.require<ISessionPack>();
  serverTicks_ = &context.require<IServerTicks>();
  serverCatalog_ = &context.require<IServerCatalog>();
  gameCatalog_ = &context.require<IGameCatalog>();
  account_ = &context.require<IAccountSession>();
  tickets_ = &context.require<ITicketLink>();
  deviceLinks_ = &context.require<IDeviceLinkFlow>();
  wallet_ = &context.require<IWallet>();
  loop_ = &context.require<FrameLoop>();
  listeners_ = &context.require<EventListeners>();
  declareSettings(context.require<engine::ConfigurationRegistry>());
  context.addWork(UPDATE_STAGE,
                  [this](engine::Duration elapsed) { update(elapsed); });
  context.addWork(RENDER_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    screens_->draw(window_->renderTarget());
                  });
}

void ClientScreensModule::declareSettings(
    engine::ConfigurationRegistry& configuration) {
  nicknameSetting_ = &configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{NICKNAME_SETTING_NAME},
      .defaultValue = launch_.nickname,
      .help = "Callsign the connection screen starts with",
      .validate = requireNickname});
  addressSetting_ = &configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{SERVER_ADDRESS_SETTING_NAME},
      .defaultValue = launch_.server.address,
      .help = "IPv4 address of the server the connection screen starts with",
      .validate = requireIpv4Address});
  portSetting_ = &configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{SERVER_PORT_SETTING_NAME},
      .defaultValue = launch_.server.port,
      .lowest = network::LOWEST_PORT,
      .highest = network::HIGHEST_PORT,
      .help = "UDP port of the server the connection screen starts with"});
}

network::Endpoint ClientScreensModule::configuredServer() const {
  return network::Endpoint{
      .address = addressSetting_->text(),
      .port = static_cast<std::uint16_t>(
          portSetting_->integer().value_or(launch_.server.port))};
}

ScreenServices ClientScreensModule::services(const network::Endpoint& server) {
  return ScreenServices{
      .settings = *settings_,
      .settingsService = *settingsService_,
      .assets = *assets_,
      .entityTable = *entityTable_,
      .sessionPack = *sessionPack_,
      .audio = *audio_,
      .window = *window_,
      .connection = *connection_,
      .waitingRoom = *waitingRoom_,
      .announcedStart = *announcedStart_,
      .shipLooks = *shipLooks_,
      .mapBallot = *mapBallot_,
      .entityMessages = *entityMessages_,
      .leaderboard = *leaderboard_,
      .globalLeaderboard = *globalLeaderboard_,
      .chat = *chat_,
      .entityTypes = *entityTypes_,
      .serverTicks = *serverTicks_,
      .serverCatalog = *serverCatalog_,
      .gameCatalog = *gameCatalog_,
      .account = *account_,
      .wallet = *wallet_,
      .tickets = *tickets_,
      .deviceLinks = *deviceLinks_,
      .signInReturn = signInReturn_,
      .choice = ServerChoice{.server = server,
                             .nickname = nicknameSetting_->text(),
                             .game = launch_.game,
                             .nextServers = {}},
      .playStart = playStart_};
}

void ClientScreensModule::start() {
  const network::Endpoint server = configuredServer();
  logger_.info("server: ", server.address, " port ", server.port);
  if (audio_ == &silence_) {
    logger_.info("no sound module: the client runs without sound");
  }
  entityTypes_ = std::make_unique<EntityTypeLooks>(*entityTable_);
  screens_ = std::make_unique<ClientScreens>(services(server), registry_);
  listenerHandle_ = listeners_->add([this](const sf::Event& event) {
    logInputEvent(logger_, event);
    screens_->handleEvent(event);
    return EventUse::Kept;
  });
}

void ClientScreensModule::stop() { listeners_->remove(listenerHandle_); }

void ClientScreensModule::update(engine::Duration elapsed) {
  screens_->update(elapsed);
  if (screens_->isEmpty()) {
    loop_->stop();
  }
}

}  // namespace rtype::client
