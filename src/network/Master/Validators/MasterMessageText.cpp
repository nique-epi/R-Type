#include "MasterMessageText.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BetsReceipt.hpp"
#include "CasinoPlayer.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomConstants.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "CreatedServer.hpp"
#include "CreatedSession.hpp"
#include "Currency.hpp"
#include "DatedStatus.hpp"
#include "EndpointText.hpp"
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "FinishedGame.hpp"
#include "FinishedGamePlayer.hpp"
#include "GameListing.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardRow.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "KopecksConstants.hpp"
#include "ListedGame.hpp"
#include "ListedGameState.hpp"
#include "ListedServer.hpp"
#include "LookItems.hpp"
#include "LookPurchase.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterRouteConstants.hpp"
#include "MovementListing.hpp"
#include "MovementReason.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "OwnedLooks.hpp"
#include "ProtocolConstants.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteBets.hpp"
#include "RouletteConstants.hpp"
#include "RoulettePhase.hpp"
#include "RoundBets.hpp"
#include "SampleListing.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"
#include "ServerSample.hpp"
#include "ServerStatus.hpp"
#include "ShipLookConstants.hpp"
#include "StatusListing.hpp"
#include "User.hpp"
#include "UserCredentials.hpp"
#include "UserRole.hpp"
#include "Wallet.hpp"
#include "WalletExchange.hpp"
#include "WalletMovement.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

/** @brief A value of a message read from JSON, with its JSON Pointer. */
struct JsonField {
  std::reference_wrapper<const nlohmann::json> value;
  nlohmann::json::json_pointer pointer;
};

/** @brief The lowest and the highest value an integer member may hold. */
struct IntegerBounds {
  std::uint64_t lowest{};
  std::uint64_t highest{};
};

[[noreturn]] void refuse(const JsonField& field) {
  throw InvalidMasterMessageException(field.pointer.to_string());
}

/** @returns The text of @p document; never throws, see registrationText(). */
std::string textOf(const nlohmann::json& document) {
  return document.dump(-1, ' ', false,
                       nlohmann::json::error_handler_t::replace);
}

/**
 * @returns The JSON object @p text holds.
 * @throws InvalidMasterMessageException naming the whole message when @p text
 * is no JSON, or no object.
 */
nlohmann::json parsedObject(std::string_view text) {
  nlohmann::json document = nlohmann::json::parse(text, nullptr, false);
  if (document.is_discarded() || !document.is_object()) {
    throw InvalidMasterMessageException(
        nlohmann::json::json_pointer{}.to_string());
  }
  return document;
}

JsonField wholeMessage(const nlohmann::json& document) {
  return JsonField{.value = std::cref(document),
                   .pointer = nlohmann::json::json_pointer{}};
}

void requireObject(const JsonField& field) {
  if (!field.value.get().is_object()) {
    refuse(field);
  }
}

std::optional<JsonField> optionalMemberOf(const JsonField& object,
                                          std::string_view member) {
  const nlohmann::json& value = object.value.get();
  const auto found = value.find(member);
  if (found == value.end()) {
    return std::nullopt;
  }
  return JsonField{.value = std::cref(*found),
                   .pointer = object.pointer / std::string{member}};
}

JsonField memberOf(const JsonField& object, std::string_view member) {
  std::optional<JsonField> found = optionalMemberOf(object, member);
  if (!found.has_value()) {
    throw InvalidMasterMessageException(
        (object.pointer / std::string{member}).to_string());
  }
  return std::move(found).value();
}

/**
 * @returns The number of elements of @p field.
 * @throws InvalidMasterMessageException when it is no array, or holds more
 * than @p maximumSize elements.
 */
std::size_t arraySizeOf(const JsonField& field, std::size_t maximumSize) {
  const nlohmann::json& value = field.value.get();
  if (!value.is_array() || value.size() > maximumSize) {
    refuse(field);
  }
  return value.size();
}

JsonField elementOf(const JsonField& array, std::size_t index) {
  return JsonField{.value = std::cref(array.value.get().at(index)),
                   .pointer = array.pointer / index};
}

/**
 * @returns The integer @p field holds; JSON writes no integer below 0 here,
 * since no member may hold one.
 */
std::uint64_t integerOf(const JsonField& field, IntegerBounds bounds) {
  const nlohmann::json& value = field.value.get();
  if (!value.is_number_unsigned()) {
    refuse(field);
  }
  const auto number = value.get<std::uint64_t>();
  if (number < bounds.lowest || number > bounds.highest) {
    refuse(field);
  }
  return number;
}

/** @returns The string @p field holds. */
std::string stringOf(const JsonField& field) {
  const nlohmann::json& value = field.value.get();
  if (!value.is_string()) {
    refuse(field);
  }
  return value.get<std::string>();
}

/** @returns The text @p field holds: 1 to @p maximumSize printable bytes. */
std::string boundedTextOf(const JsonField& field, std::size_t maximumSize) {
  std::string text = stringOf(field);
  if (!isValidBoundedText(text, maximumSize)) {
    refuse(field);
  }
  return text;
}

/** @returns The password @p field holds: MINIMUM_PASSWORD_SIZE to
 * MAXIMUM_PASSWORD_SIZE bytes of printable UTF-8. */
std::string passwordOf(const JsonField& field) {
  std::string password = stringOf(field);
  if (password.size() < MINIMUM_PASSWORD_SIZE ||
      password.size() > MAXIMUM_PASSWORD_SIZE || !isPrintableUtf8(password)) {
    refuse(field);
  }
  return password;
}

nlohmann::json userDocument(const User& account) {
  nlohmann::json document;
  document[ID_MEMBER] = account.id;
  document[NICKNAME_MEMBER] = account.nickname;
  document[ROLE_MEMBER] = userRoleText(account.role);
  return document;
}

/** @returns The nickname, or the difficulty, @p field holds. */
std::string nicknameOf(const JsonField& field) {
  std::string nickname = stringOf(field);
  if (!isValidNickname(nickname)) {
    refuse(field);
  }
  return nickname;
}

std::optional<std::string> addressOf(const JsonField& registration) {
  const std::optional<JsonField> field =
      optionalMemberOf(registration, ADDRESS_MEMBER);
  if (!field.has_value()) {
    return std::nullopt;
  }
  try {
    return ipv4AddressFromText(stringOf(*field));
  } catch (const InvalidIpv4AddressException&) {
    refuse(*field);
  }
}

/** @returns The probeToken of @p heartbeat, nothing when it has none. */
std::optional<std::uint64_t> probeTokenOf(const JsonField& heartbeat) {
  const std::optional<JsonField> token =
      optionalMemberOf(heartbeat, PROBE_TOKEN_MEMBER);
  if (!token.has_value()) {
    return std::nullopt;
  }
  return integerOf(*token, IntegerBounds{.lowest = LOWEST_PROBE_TOKEN,
                                         .highest = HIGHEST_PROBE_TOKEN});
}

/**
 * @returns The value of Enum that @p texts, one text per value in the order of
 * the enum, writes as @p text; nothing for any other text.
 */
template <typename Enum, std::size_t Count>
std::optional<Enum> valueOfText(
    const std::array<std::string_view, Count>& texts, std::string_view text) {
  for (std::size_t index = 0; index < texts.size(); ++index) {
    if (text == texts.at(index)) {
      return static_cast<Enum>(index);
    }
  }
  return std::nullopt;
}

HostedGameState stateOf(const JsonField& field) {
  const std::optional<HostedGameState> state =
      valueOfText<HostedGameState>(HOSTED_GAME_STATE_TEXTS, stringOf(field));
  if (!state.has_value()) {
    refuse(field);
  }
  return *state;
}

/** @returns The nicknames of the protocol @p field lists, @p maximumCount at
 * most. */
std::vector<std::string> nicknamesOf(const JsonField& field,
                                     std::size_t maximumCount) {
  const std::size_t count = arraySizeOf(field, maximumCount);
  std::vector<std::string> nicknames;
  nicknames.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    nicknames.push_back(nicknameOf(elementOf(field, index)));
  }
  return nicknames;
}

/** @returns Whether @p field holds true; it must hold a JSON boolean. */
bool booleanOf(const JsonField& field) {
  const nlohmann::json& value = field.value.get();
  if (!value.is_boolean()) {
    refuse(field);
  }
  return value.get<bool>();
}

/** @returns The lives @p game gives its players, LOWEST_GAME_LIVES to
 * HIGHEST_GAME_LIVES. */
std::uint8_t livesOf(const JsonField& game) {
  return static_cast<std::uint8_t>(
      integerOf(memberOf(game, LIVES_MEMBER),
                IntegerBounds{.lowest = LOWEST_GAME_LIVES,
                              .highest = HIGHEST_GAME_LIVES}));
}

/** @returns The difficulties @p message holds: @p fewest to
 * MAXIMUM_DIFFICULTIES distinct names, each following the rules of a
 * nickname. */
std::vector<std::string> difficultiesOf(const JsonField& message,
                                        std::size_t fewest) {
  const JsonField field = memberOf(message, DIFFICULTIES_MEMBER);
  std::vector<std::string> names = nicknamesOf(field, MAXIMUM_DIFFICULTIES);
  if (names.size() < fewest) {
    refuse(field);
  }
  for (std::size_t later = 1; later < names.size(); ++later) {
    const std::span<const std::string> earlier = std::span(names).first(later);
    if (std::ranges::find(earlier, names.at(later)) != earlier.end()) {
      refuse(elementOf(field, later));
    }
  }
  return names;
}

std::vector<std::string> playersOf(const JsonField& game) {
  return nicknamesOf(memberOf(game, PLAYERS_MEMBER), MAXIMUM_PLAYERS);
}

/** @returns The number of the game @p game, which a heartbeat and a listing
 * write alike. */
std::uint16_t gameNumberOf(const JsonField& game) {
  return static_cast<std::uint16_t>(
      integerOf(memberOf(game, NUMBER_MEMBER),
                IntegerBounds{.lowest = FIRST_HOSTED_GAME_NUMBER,
                              .highest = MAXIMUM_HOSTED_GAMES}));
}

/** @returns The name of the game @p game, which a heartbeat and a listing
 * write alike. */
std::string gameNameOf(const JsonField& game) {
  return boundedTextOf(memberOf(game, NAME_MEMBER), MAXIMUM_GAME_NAME_SIZE);
}

HostedGame gameOf(const JsonField& game) {
  requireObject(game);
  return HostedGame{
      .number = gameNumberOf(game),
      .name = gameNameOf(game),
      .state = stateOf(memberOf(game, STATE_MEMBER)),
      .difficulty = nicknameOf(memberOf(game, DIFFICULTY_MEMBER)),
      .lives = livesOf(game),
      .friendlyFire = booleanOf(memberOf(game, FRIENDLY_FIRE_MEMBER)),
      .players = playersOf(game)};
}

/**
 * @throws InvalidMasterMessageException naming the number of the first game
 * of @p hosted whose number an earlier game already has.
 */
template <typename Game>
void requireDistinctNumbers(const JsonField& games,
                            const std::vector<Game>& hosted) {
  for (std::size_t later = 0; later < hosted.size(); ++later) {
    for (std::size_t earlier = 0; earlier < later; ++earlier) {
      if (hosted.at(earlier).number == hosted.at(later).number) {
        refuse(memberOf(elementOf(games, later), NUMBER_MEMBER));
      }
    }
  }
}

ListedGameState listedStateOf(const JsonField& field) {
  const std::optional<ListedGameState> state =
      valueOfText<ListedGameState>(LISTED_GAME_STATE_TEXTS, stringOf(field));
  if (!state.has_value()) {
    refuse(field);
  }
  return *state;
}

ListedGame listedGameOf(const JsonField& game) {
  requireObject(game);
  return ListedGame{
      .number = gameNumberOf(game),
      .name = gameNameOf(game),
      .state = listedStateOf(memberOf(game, STATE_MEMBER)),
      .playerCount = static_cast<std::uint8_t>(
          integerOf(memberOf(game, PLAYER_COUNT_MEMBER),
                    IntegerBounds{.lowest = MINIMUM_LISTED_PLAYERS,
                                  .highest = MAXIMUM_PLAYERS})),
      .difficulty = nicknameOf(memberOf(game, DIFFICULTY_MEMBER)),
      .lives = livesOf(game),
      .friendlyFire = booleanOf(memberOf(game, FRIENDLY_FIRE_MEMBER))};
}

nlohmann::json listedGameDocument(const ListedGame& game) {
  nlohmann::json document;
  document[NUMBER_MEMBER] = game.number;
  document[NAME_MEMBER] = game.name;
  document[STATE_MEMBER] =
      LISTED_GAME_STATE_TEXTS.at(static_cast<std::size_t>(game.state));
  document[PLAYER_COUNT_MEMBER] = game.playerCount;
  document[DIFFICULTY_MEMBER] = game.difficulty;
  document[LIVES_MEMBER] = game.lives;
  document[FRIENDLY_FIRE_MEMBER] = game.friendlyFire;
  return document;
}

std::vector<HostedGame> gamesOf(const JsonField& heartbeat) {
  const JsonField games = memberOf(heartbeat, GAMES_MEMBER);
  const std::size_t count = arraySizeOf(games, MAXIMUM_HOSTED_GAMES);
  std::vector<HostedGame> hosted;
  hosted.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    hosted.push_back(gameOf(elementOf(games, index)));
  }
  requireDistinctNumbers(games, hosted);
  return hosted;
}

/**
 * @returns The measure @p member of @p measures, empty when it is left out.
 * @throws InvalidMasterMessageException when it is no number, or lies outside
 * 0 to @p highest.
 */
std::optional<double> measureOf(const JsonField& measures,
                                std::string_view member, double highest) {
  const std::optional<JsonField> field = optionalMemberOf(measures, member);
  if (!field.has_value()) {
    return std::nullopt;
  }
  const nlohmann::json& value = field->value.get();
  if (!value.is_number()) {
    refuse(*field);
  }
  const auto measure = value.get<double>();
  if (!std::isfinite(measure) || measure < 0.0 || measure > highest) {
    refuse(*field);
  }
  return measure;
}

ServerMeasures measuresOf(const JsonField& heartbeat) {
  const JsonField measures = memberOf(heartbeat, MEASURES_MEMBER);
  requireObject(measures);
  return ServerMeasures{
      .tickP50Milliseconds =
          measureOf(measures, TICK_P50_MEMBER, MAXIMUM_MEASURE),
      .tickP95Milliseconds =
          measureOf(measures, TICK_P95_MEMBER, MAXIMUM_MEASURE),
      .receivedBytesPerSecond =
          measureOf(measures, RECEIVED_BYTES_MEMBER, MAXIMUM_MEASURE),
      .sentBytesPerSecond =
          measureOf(measures, SENT_BYTES_MEMBER, MAXIMUM_MEASURE),
      .lossPercent =
          measureOf(measures, LOSS_PERCENT_MEMBER, MAXIMUM_LOSS_PERCENT),
      .averagePingMilliseconds =
          measureOf(measures, AVERAGE_PING_MEMBER, MAXIMUM_MEASURE),
      .cpuPercent =
          measureOf(measures, CPU_PERCENT_MEMBER, MAXIMUM_CPU_PERCENT),
      .usedMemoryBytes =
          measureOf(measures, USED_MEMORY_MEMBER, MAXIMUM_MEASURE),
      .totalMemoryBytes =
          measureOf(measures, TOTAL_MEMORY_MEMBER, MAXIMUM_MEASURE)};
}

nlohmann::json gameDocument(const HostedGame& game) {
  nlohmann::json document;
  document[NUMBER_MEMBER] = game.number;
  document[NAME_MEMBER] = game.name;
  document[STATE_MEMBER] =
      HOSTED_GAME_STATE_TEXTS.at(static_cast<std::size_t>(game.state));
  document[DIFFICULTY_MEMBER] = game.difficulty;
  document[LIVES_MEMBER] = game.lives;
  document[FRIENDLY_FIRE_MEMBER] = game.friendlyFire;
  document[PLAYERS_MEMBER] = game.players;
  return document;
}

void addMeasure(nlohmann::json& document, std::string_view member,
                std::optional<double> measure) {
  if (measure.has_value()) {
    document[member] = *measure;
  }
}

nlohmann::json measuresDocument(const ServerMeasures& measures) {
  nlohmann::json document = nlohmann::json::object();
  addMeasure(document, TICK_P50_MEMBER, measures.tickP50Milliseconds);
  addMeasure(document, TICK_P95_MEMBER, measures.tickP95Milliseconds);
  addMeasure(document, RECEIVED_BYTES_MEMBER, measures.receivedBytesPerSecond);
  addMeasure(document, SENT_BYTES_MEMBER, measures.sentBytesPerSecond);
  addMeasure(document, LOSS_PERCENT_MEMBER, measures.lossPercent);
  addMeasure(document, AVERAGE_PING_MEMBER, measures.averagePingMilliseconds);
  addMeasure(document, CPU_PERCENT_MEMBER, measures.cpuPercent);
  addMeasure(document, USED_MEMORY_MEMBER, measures.usedMemoryBytes);
  addMeasure(document, TOTAL_MEMORY_MEMBER, measures.totalMemoryBytes);
  return document;
}

/**
 * @returns An object whose one member, @p member, lists each of @p items as
 * @p documentOf writes it: the shape of every list the master answers.
 */
template <typename Item, typename Document>
nlohmann::json listingDocument(std::string_view member,
                               const std::vector<Item>& items,
                               Document documentOf) {
  nlohmann::json document;
  document[member] = nlohmann::json::array();
  for (const Item& item : items) {
    document[member].push_back(documentOf(item));
  }
  return document;
}

/** @returns The text of listingDocument(). */
template <typename Item, typename Document>
std::string listingText(std::string_view member, const std::vector<Item>& items,
                        Document documentOf) {
  return textOf(listingDocument(member, items, documentOf));
}

/** @returns The members of @p registration, its address left out when it is
 * empty: what a registration and a listed server write alike. */
nlohmann::json registrationDocument(const ServerRegistration& registration) {
  nlohmann::json document;
  document[NAME_MEMBER] = registration.name;
  if (registration.address.has_value()) {
    document[ADDRESS_MEMBER] = *registration.address;
  }
  document[UDP_PORT_MEMBER] = registration.udpPort;
  document[GAME_VERSION_MEMBER] = registration.gameVersion;
  document[PROTOCOL_VERSION_MEMBER] = registration.protocolVersion;
  document[MAXIMUM_GAMES_MEMBER] = registration.maximumGames;
  document[MAXIMUM_PLAYERS_PER_GAME_MEMBER] =
      registration.maximumPlayersPerGame;
  return document;
}

nlohmann::json listedServerDocument(const ListedServer& server) {
  nlohmann::json document = registrationDocument(server.registration);
  document[ID_MEMBER] = server.id;
  document[STATUS_MEMBER] = serverStatusText(server.status);
  document[GAME_COUNT_MEMBER] = server.gameCount;
  document[PLAYER_COUNT_MEMBER] = server.playerCount;
  return document;
}

/** @returns The registration whose members @p registration holds, each
 * checked as registrationFromText() checks it. */
ServerRegistration registrationOf(const JsonField& registration) {
  return ServerRegistration{
      .name = boundedTextOf(memberOf(registration, NAME_MEMBER),
                            MAXIMUM_SERVER_NAME_SIZE),
      .address = addressOf(registration),
      .udpPort = static_cast<std::uint16_t>(integerOf(
          memberOf(registration, UDP_PORT_MEMBER),
          IntegerBounds{.lowest = LOWEST_PORT, .highest = HIGHEST_PORT})),
      .gameVersion = boundedTextOf(memberOf(registration, GAME_VERSION_MEMBER),
                                   MAXIMUM_GAME_VERSION_SIZE),
      .protocolVersion = static_cast<std::uint8_t>(
          integerOf(memberOf(registration, PROTOCOL_VERSION_MEMBER),
                    IntegerBounds{.lowest = LOWEST_PROTOCOL_VERSION,
                                  .highest = HIGHEST_PROTOCOL_VERSION})),
      .maximumGames = static_cast<std::uint16_t>(
          integerOf(memberOf(registration, MAXIMUM_GAMES_MEMBER),
                    IntegerBounds{.lowest = MINIMUM_HOSTED_GAMES,
                                  .highest = MAXIMUM_HOSTED_GAMES})),
      .maximumPlayersPerGame = static_cast<std::uint8_t>(
          integerOf(memberOf(registration, MAXIMUM_PLAYERS_PER_GAME_MEMBER),
                    IntegerBounds{.lowest = MINIMUM_PLAYERS_PER_GAME,
                                  .highest = MAXIMUM_PLAYERS}))};
}

/** @returns The status @p field writes, one of SERVER_STATUS_TEXTS. */
ServerStatus listedStatusOf(const JsonField& field) {
  const std::optional<ServerStatus> status = serverStatusOf(stringOf(field));
  if (!status.has_value()) {
    refuse(field);
  }
  return *status;
}

/** @returns The count @p field holds, 0 to @p highest. */
std::uint16_t listedCountOf(const JsonField& field, std::uint64_t highest) {
  return static_cast<std::uint16_t>(
      integerOf(field, IntegerBounds{.lowest = 0, .highest = highest}));
}

ListedServer listedServerOf(const JsonField& server) {
  requireObject(server);
  return ListedServer{
      .id = static_cast<std::int64_t>(
          integerOf(memberOf(server, ID_MEMBER),
                    IntegerBounds{.lowest = FIRST_SERVER_ID,
                                  .highest = HIGHEST_SERVER_ID})),
      .registration = registrationOf(server),
      .status = listedStatusOf(memberOf(server, STATUS_MEMBER)),
      .gameCount = listedCountOf(memberOf(server, GAME_COUNT_MEMBER),
                                 MAXIMUM_HOSTED_GAMES),
      .playerCount =
          listedCountOf(memberOf(server, PLAYER_COUNT_MEMBER),
                        std::uint64_t{MAXIMUM_HOSTED_GAMES} * MAXIMUM_PLAYERS)};
}

nlohmann::json sampleDocument(const ServerSample& sample) {
  nlohmann::json document;
  document[TAKEN_AT_MEMBER] = sample.takenAt.time_since_epoch().count();
  document[GAME_COUNT_MEMBER] = sample.gameCount;
  document[PLAYER_COUNT_MEMBER] = sample.playerCount;
  document[MEASURES_MEMBER] = measuresDocument(sample.measures);
  return document;
}

nlohmann::json statusChangeDocument(const DatedStatus& change) {
  nlohmann::json document;
  document[SINCE_MEMBER] = change.since.time_since_epoch().count();
  document[STATUS_MEMBER] = serverStatusText(change.status);
  return document;
}

std::string serverKeyOf(const JsonField& field) {
  std::string key = stringOf(field);
  if (!isWellFormedServerKey(key)) {
    refuse(field);
  }
  return key;
}

/** @returns Whether every character of @p text is one of @p digits. */
bool isMadeOf(std::string_view text, std::string_view digits) {
  return std::ranges::all_of(text, [digits](char digit) {
    return std::ranges::find(digits, digit) != digits.end();
  });
}

/** @returns The date the game @p game ended. */
std::chrono::sys_seconds playedAtOf(const JsonField& game) {
  const std::uint64_t seconds = integerOf(
      memberOf(game, PLAYED_AT_MEMBER),
      IntegerBounds{.lowest = EARLIEST_PLAYED_AT, .highest = LATEST_PLAYED_AT});
  return std::chrono::sys_seconds{
      std::chrono::seconds{static_cast<std::int64_t>(seconds)}};
}

/** @returns The account of @p player, nothing for a guest. */
std::optional<std::string> accountIdOf(const JsonField& player) {
  const std::optional<JsonField> account =
      optionalMemberOf(player, ACCOUNT_ID_MEMBER);
  if (!account.has_value()) {
    return std::nullopt;
  }
  return boundedTextOf(*account, MAXIMUM_ACCOUNT_ID_SIZE);
}

/** @returns The name of a look @p field holds: a name of
 * identifiers::SHIP_LOOK_ITEM_NAMES. */
std::string lookNameOf(const JsonField& field) {
  std::string name = stringOf(field);
  if (!lookItemOfName(name).has_value()) {
    refuse(field);
  }
  return name;
}

/** @returns The look items @p player won, at most MAXIMUM_WON_LOOKS; none
 * when it does not say, as a server older than the member reports. */
std::vector<std::string> wonLooksOf(const JsonField& player) {
  const std::optional<JsonField> looks =
      optionalMemberOf(player, WON_LOOKS_MEMBER);
  if (!looks.has_value()) {
    return {};
  }
  const std::size_t count = arraySizeOf(*looks, MAXIMUM_WON_LOOKS);
  std::vector<std::string> read;
  read.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    read.push_back(lookNameOf(elementOf(*looks, index)));
  }
  return read;
}

/** @returns Whether @p game says it was won; false when it does not say, as
 * a server older than the member reports. */
bool victoryOf(const JsonField& game) {
  const std::optional<JsonField> victory =
      optionalMemberOf(game, VICTORY_MEMBER);
  return victory.has_value() && booleanOf(*victory);
}

FinishedGamePlayer finishedGamePlayerOf(const JsonField& player) {
  requireObject(player);
  return FinishedGamePlayer{
      .nickname = nicknameOf(memberOf(player, NICKNAME_MEMBER)),
      .accountId = accountIdOf(player),
      .score = static_cast<std::uint32_t>(integerOf(
          memberOf(player, SCORE_MEMBER),
          IntegerBounds{.lowest = LOWEST_SCORE, .highest = HIGHEST_SCORE})),
      .wonLooks = wonLooksOf(player)};
}

std::vector<FinishedGamePlayer> finishedGamePlayersOf(const JsonField& game) {
  const JsonField players = memberOf(game, PLAYERS_MEMBER);
  const std::size_t count = arraySizeOf(players, MAXIMUM_PLAYERS);
  if (count < FEWEST_FINISHED_GAME_PLAYERS) {
    refuse(players);
  }
  std::vector<FinishedGamePlayer> read;
  read.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    read.push_back(finishedGamePlayerOf(elementOf(players, index)));
  }
  return read;
}

/**
 * @brief Refuses a game that names more players than started it, since a game
 * server lets nobody in once a game runs, or one nickname twice, compared as a
 * game server compares them (nicknameKey()): the second row would be lost.
 */
void requirePlayersOfTheGame(const JsonField& game, const FinishedGame& read) {
  const JsonField players = memberOf(game, PLAYERS_MEMBER);
  if (read.players.size() > read.playerCount) {
    refuse(players);
  }
  std::vector<std::string> keys;
  for (std::size_t index = 0; index < read.players.size(); ++index) {
    std::string key = nicknameKey(read.players[index].nickname);
    if (std::ranges::find(keys, key) != keys.end()) {
      refuse(memberOf(elementOf(players, index), NICKNAME_MEMBER));
    }
    keys.push_back(std::move(key));
  }
}

nlohmann::json finishedGamePlayerDocument(const FinishedGamePlayer& player) {
  nlohmann::json document;
  document[NICKNAME_MEMBER] = player.nickname;
  if (player.accountId.has_value()) {
    document[ACCOUNT_ID_MEMBER] = *player.accountId;
  }
  document[SCORE_MEMBER] = player.score;
  document[WON_LOOKS_MEMBER] = player.wonLooks;
  return document;
}

nlohmann::json globalLeaderboardRowDocument(const GlobalLeaderboardRow& row) {
  nlohmann::json document;
  document[RANK_MEMBER] = row.rank;
  document[NICKNAME_MEMBER] = row.nickname;
  document[SCORE_MEMBER] = row.score;
  document[PLAYED_AT_MEMBER] = row.playedAt.time_since_epoch().count();
  document[SERVER_ID_MEMBER] = row.serverId;
  document[SERVER_NAME_MEMBER] = row.serverName;
  return document;
}

GlobalLeaderboardRow globalLeaderboardRowOf(const JsonField& row) {
  requireObject(row);
  return GlobalLeaderboardRow{
      .rank = static_cast<std::int64_t>(integerOf(
          memberOf(row, RANK_MEMBER),
          IntegerBounds{.lowest = LOWEST_RANK, .highest = HIGHEST_RANK})),
      .nickname = nicknameOf(memberOf(row, NICKNAME_MEMBER)),
      .score = static_cast<std::uint32_t>(integerOf(
          memberOf(row, SCORE_MEMBER),
          IntegerBounds{.lowest = LOWEST_SCORE, .highest = HIGHEST_SCORE})),
      .playedAt = playedAtOf(row),
      .serverId = static_cast<std::int64_t>(
          integerOf(memberOf(row, SERVER_ID_MEMBER),
                    IntegerBounds{.lowest = FIRST_SERVER_ID,
                                  .highest = HIGHEST_SERVER_ID})),
      .serverName = boundedTextOf(memberOf(row, SERVER_NAME_MEMBER),
                                  MAXIMUM_SERVER_NAME_SIZE)};
}

}  // namespace

bool isWellFormedServerKey(std::string_view text) {
  return text.size() == SERVER_KEY_SIZE &&
         text.starts_with(SERVER_KEY_PREFIX) &&
         isMadeOf(text.substr(SERVER_KEY_PREFIX.size()), SERVER_KEY_DIGITS);
}

bool isWellFormedGameId(std::string_view text) {
  return text.size() == GAME_ID_SIZE && isMadeOf(text, GAME_ID_DIGITS);
}

std::string finishedGameText(const FinishedGame& game) {
  nlohmann::json document =
      listingDocument(PLAYERS_MEMBER, game.players, finishedGamePlayerDocument);
  document[PLAYED_AT_MEMBER] = game.playedAt.time_since_epoch().count();
  document[DIFFICULTY_MEMBER] = game.difficulty;
  document[PLAYER_COUNT_MEMBER] = game.playerCount;
  document[VANILLA_MEMBER] = game.vanilla;
  document[VICTORY_MEMBER] = game.victory;
  return textOf(document);
}

FinishedGame finishedGameFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField game = wholeMessage(document);
  FinishedGame read{.playedAt = playedAtOf(game),
                    .difficulty = nicknameOf(memberOf(game, DIFFICULTY_MEMBER)),
                    .playerCount = static_cast<std::uint8_t>(integerOf(
                        memberOf(game, PLAYER_COUNT_MEMBER),
                        IntegerBounds{.lowest = MINIMUM_PLAYERS_PER_GAME,
                                      .highest = MAXIMUM_PLAYERS})),
                    .vanilla = booleanOf(memberOf(game, VANILLA_MEMBER)),
                    .players = finishedGamePlayersOf(game),
                    .victory = victoryOf(game)};
  requirePlayersOfTheGame(game, read);
  return read;
}

std::string globalLeaderboardText(const GlobalLeaderboard& leaderboard) {
  nlohmann::json document = listingDocument(SCORES_MEMBER, leaderboard.scores,
                                            globalLeaderboardRowDocument);
  if (leaderboard.player.has_value()) {
    document[PLAYER_MEMBER] = globalLeaderboardRowDocument(*leaderboard.player);
  }
  return textOf(document);
}

GlobalLeaderboard globalLeaderboardFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField leaderboard = wholeMessage(document);
  const JsonField scores = memberOf(leaderboard, SCORES_MEMBER);
  const std::size_t count =
      arraySizeOf(scores, static_cast<std::size_t>(MAXIMUM_PAGE_SIZE));
  GlobalLeaderboard read;
  read.scores.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    read.scores.push_back(globalLeaderboardRowOf(elementOf(scores, index)));
  }
  if (const std::optional<JsonField> player =
          optionalMemberOf(leaderboard, PLAYER_MEMBER)) {
    read.player = globalLeaderboardRowOf(*player);
  }
  return read;
}

std::string serverCreationText(const ServerCreation& creation) {
  nlohmann::json document;
  document[NAME_MEMBER] = creation.name;
  return textOf(document);
}

ServerCreation serverCreationFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField creation = wholeMessage(document);
  return ServerCreation{.name = boundedTextOf(memberOf(creation, NAME_MEMBER),
                                              MAXIMUM_SERVER_NAME_SIZE)};
}

std::string createdServerText(const CreatedServer& created) {
  nlohmann::json document;
  document[ID_MEMBER] = created.id;
  document[NAME_MEMBER] = created.name;
  document[KEY_MEMBER] = created.key;
  return textOf(document);
}

CreatedServer createdServerFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField created = wholeMessage(document);
  return CreatedServer{.id = static_cast<std::int64_t>(integerOf(
                           memberOf(created, ID_MEMBER),
                           IntegerBounds{.lowest = FIRST_SERVER_ID,
                                         .highest = HIGHEST_SERVER_ID})),
                       .name = boundedTextOf(memberOf(created, NAME_MEMBER),
                                             MAXIMUM_SERVER_NAME_SIZE),
                       .key = serverKeyOf(memberOf(created, KEY_MEMBER))};
}

std::string registrationText(const ServerRegistration& registration) {
  nlohmann::json document = registrationDocument(registration);
  document[PROBE_SECRET_MEMBER] = registration.probeSecret;
  return textOf(document);
}

ServerRegistration registrationFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField registration = wholeMessage(document);
  ServerRegistration read = registrationOf(registration);
  read.probeSecret = integerOf(memberOf(registration, PROBE_SECRET_MEMBER),
                               IntegerBounds{.lowest = LOWEST_PROBE_SECRET,
                                             .highest = HIGHEST_PROBE_SECRET});
  return read;
}

std::string heartbeatText(const ServerHeartbeat& heartbeat) {
  nlohmann::json document =
      listingDocument(GAMES_MEMBER, heartbeat.games, gameDocument);
  document[DIFFICULTIES_MEMBER] = heartbeat.difficulties;
  document[MEASURES_MEMBER] = measuresDocument(heartbeat.measures);
  if (heartbeat.probeToken.has_value()) {
    document[PROBE_TOKEN_MEMBER] = *heartbeat.probeToken;
  }
  return textOf(document);
}

ServerHeartbeat heartbeatFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField heartbeat = wholeMessage(document);
  return ServerHeartbeat{
      .games = gamesOf(heartbeat),
      .difficulties = difficultiesOf(heartbeat, FEWEST_HEARTBEAT_DIFFICULTIES),
      .measures = measuresOf(heartbeat),
      .probeToken = probeTokenOf(heartbeat)};
}

std::string gameListingText(const GameListing& listing) {
  nlohmann::json document =
      listingDocument(GAMES_MEMBER, listing.games, listedGameDocument);
  document[DIFFICULTIES_MEMBER] = listing.difficulties;
  return textOf(document);
}

GameListing gameListingFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField games = memberOf(wholeMessage(document), GAMES_MEMBER);
  const std::size_t count = arraySizeOf(games, MAXIMUM_HOSTED_GAMES);
  GameListing listing;
  listing.difficulties =
      difficultiesOf(wholeMessage(document), FEWEST_LISTED_DIFFICULTIES);
  listing.games.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    listing.games.push_back(listedGameOf(elementOf(games, index)));
  }
  requireDistinctNumbers(games, listing.games);
  return listing;
}

std::string_view serverStatusText(ServerStatus status) {
  return SERVER_STATUS_TEXTS.at(static_cast<std::size_t>(status));
}

std::optional<ServerStatus> serverStatusOf(std::string_view text) {
  return valueOfText<ServerStatus>(SERVER_STATUS_TEXTS, text);
}

std::string serverListingText(const ServerListing& listing) {
  return listingText(SERVERS_MEMBER, listing.servers, listedServerDocument);
}

ServerListing serverListingFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField servers = memberOf(wholeMessage(document), SERVERS_MEMBER);
  const std::size_t count =
      arraySizeOf(servers, static_cast<std::size_t>(MAXIMUM_PAGE_SIZE));
  ServerListing listing;
  listing.servers.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    listing.servers.push_back(listedServerOf(elementOf(servers, index)));
  }
  return listing;
}

std::string sampleListingText(const SampleListing& listing) {
  return listingText(SAMPLES_MEMBER, listing.samples, sampleDocument);
}

std::string statusListingText(const StatusListing& listing) {
  return listingText(STATUSES_MEMBER, listing.statuses, statusChangeDocument);
}

std::string userCredentialsText(const UserCredentials& credentials) {
  nlohmann::json document;
  document[NICKNAME_MEMBER] = credentials.nickname;
  document[PASSWORD_MEMBER] = credentials.password;
  return textOf(document);
}

UserCredentials userCredentialsFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField credentials = wholeMessage(document);
  return UserCredentials{
      .nickname = nicknameOf(memberOf(credentials, NICKNAME_MEMBER)),
      .password = passwordOf(memberOf(credentials, PASSWORD_MEMBER))};
}

std::string_view userRoleText(UserRole role) {
  return USER_ROLE_TEXTS.at(static_cast<std::size_t>(role));
}

std::optional<UserRole> userRoleOf(std::string_view text) {
  return valueOfText<UserRole>(USER_ROLE_TEXTS, text);
}

std::string userText(const User& account) {
  return textOf(userDocument(account));
}

std::string createdSessionText(const CreatedSession& session) {
  nlohmann::json document;
  document[TOKEN_MEMBER] = session.token;
  document[EXPIRES_AT_MEMBER] = session.expiresAt.time_since_epoch().count();
  document[USER_MEMBER] = userDocument(session.user);
  return textOf(document);
}

bool isWellFormedSessionToken(std::string_view text) {
  return text.size() == SESSION_TOKEN_SIZE &&
         text.starts_with(SESSION_TOKEN_PREFIX) &&
         isMadeOf(text.substr(SESSION_TOKEN_PREFIX.size()),
                  LOWERCASE_HEX_DIGITS);
}

std::string problemDetailOf(std::string_view text) {
  const nlohmann::json document = nlohmann::json::parse(text, nullptr, false);
  if (!document.is_object()) {
    return std::string{};
  }
  const auto detail = document.find(std::string{PROBLEM_DETAIL_MEMBER});
  if (detail == document.end() || !detail->is_string()) {
    return std::string{};
  }
  return detail->get<std::string>();
}

namespace {

nlohmann::json walletDocument(const Wallet& wallet) {
  nlohmann::json document;
  document[KOPECKS_MEMBER] = wallet.kopecks;
  document[CHIPS_MEMBER] = wallet.chips;
  return document;
}

Wallet walletOf(const JsonField& wallet) {
  requireObject(wallet);
  return Wallet{.kopecks = static_cast<std::uint32_t>(integerOf(
                    memberOf(wallet, KOPECKS_MEMBER),
                    IntegerBounds{.lowest = 0, .highest = MAXIMUM_KOPECKS})),
                .chips = static_cast<std::uint32_t>(integerOf(
                    memberOf(wallet, CHIPS_MEMBER),
                    IntegerBounds{.lowest = 0, .highest = MAXIMUM_CHIPS}))};
}

Currency currencyIn(const JsonField& field) {
  const std::optional<Currency> currency = currencyOf(stringOf(field));
  if (!currency.has_value()) {
    refuse(field);
  }
  return *currency;
}

std::uint32_t exchangedAmountOf(const JsonField& field) {
  return static_cast<std::uint32_t>(integerOf(
      field, IntegerBounds{.lowest = 0, .highest = MAXIMUM_EXCHANGE_AMOUNT}));
}

nlohmann::json movementDocument(const WalletMovement& movement) {
  nlohmann::json document;
  document[ID_MEMBER] = movement.id;
  document[CURRENCY_MEMBER] = currencyText(movement.currency);
  document[AMOUNT_MEMBER] = movement.amount;
  document[REASON_MEMBER] = movementReasonText(movement.reason);
  document[MADE_AT_MEMBER] = movement.madeAt.time_since_epoch().count();
  return document;
}

nlohmann::json walletExchangeDocument(const WalletExchange& exchange) {
  nlohmann::json document;
  document[ID_MEMBER] = exchange.id;
  document[FROM_MEMBER] = currencyText(exchange.from);
  document[AMOUNT_MEMBER] = exchange.amount;
  document[MADE_AT_MEMBER] = exchange.madeAt.time_since_epoch().count();
  return document;
}

/** @returns The whole number @p field holds, 0 to the largest of 64 signed
 * bits: a number of the journal, or a date in seconds since 1970. */
std::int64_t nonNegativeIntegerOf(const JsonField& field) {
  return static_cast<std::int64_t>(integerOf(
      field, IntegerBounds{.lowest = 0,
                           .highest = static_cast<std::uint64_t>(
                               std::numeric_limits<std::int64_t>::max())}));
}

WalletExchange walletExchangeOf(const JsonField& exchange) {
  return WalletExchange{
      .id = nonNegativeIntegerOf(memberOf(exchange, ID_MEMBER)),
      .from = currencyIn(memberOf(exchange, FROM_MEMBER)),
      .amount = exchangedAmountOf(memberOf(exchange, AMOUNT_MEMBER)),
      .madeAt = std::chrono::sys_seconds{std::chrono::seconds{
          nonNegativeIntegerOf(memberOf(exchange, MADE_AT_MEMBER))}}};
}

}  // namespace

std::string walletText(const Wallet& wallet) {
  return textOf(walletDocument(wallet));
}

Wallet walletFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  return walletOf(wholeMessage(document));
}

std::string movementListingText(const MovementListing& listing) {
  return listingText(MOVEMENTS_MEMBER, listing.movements, movementDocument);
}

std::string exchangeRequestText(const ExchangeRequest& request) {
  nlohmann::json document;
  document[FROM_MEMBER] = currencyText(request.from);
  document[AMOUNT_MEMBER] = request.amount;
  return textOf(document);
}

ExchangeRequest exchangeRequestFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField request = wholeMessage(document);
  return ExchangeRequest{
      .from = currencyIn(memberOf(request, FROM_MEMBER)),
      .amount = exchangedAmountOf(memberOf(request, AMOUNT_MEMBER))};
}

std::string walletExchangeText(const WalletExchange& exchange) {
  return textOf(walletExchangeDocument(exchange));
}

std::string exchangeReceiptText(const ExchangeReceipt& receipt) {
  nlohmann::json document = walletExchangeDocument(receipt.exchange);
  document[WALLET_MEMBER] = walletDocument(receipt.wallet);
  return textOf(document);
}

ExchangeReceipt exchangeReceiptFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField receipt = wholeMessage(document);
  return ExchangeReceipt{.exchange = walletExchangeOf(receipt),
                         .wallet = walletOf(memberOf(receipt, WALLET_MEMBER))};
}

std::string ownedLooksText(const OwnedLooks& looks) {
  nlohmann::json document;
  document[ITEMS_MEMBER] = looks.items;
  return textOf(document);
}

OwnedLooks ownedLooksFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField items = memberOf(wholeMessage(document), ITEMS_MEMBER);
  const std::size_t count =
      arraySizeOf(items, identifiers::SHIP_LOOK_ITEM_COUNT);
  OwnedLooks looks;
  looks.items.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    looks.items.push_back(lookNameOf(elementOf(items, index)));
  }
  return looks;
}

std::string lookPurchaseText(const LookPurchase& purchase) {
  nlohmann::json document;
  document[ITEM_MEMBER] = purchase.item;
  document[WALLET_MEMBER] = walletDocument(purchase.wallet);
  return textOf(document);
}

LookPurchase lookPurchaseFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField purchase = wholeMessage(document);
  return LookPurchase{.item = lookNameOf(memberOf(purchase, ITEM_MEMBER)),
                      .wallet = walletOf(memberOf(purchase, WALLET_MEMBER))};
}

std::string_view movementReasonText(MovementReason reason) {
  return MOVEMENT_REASON_TEXTS.at(static_cast<std::size_t>(reason));
}

std::optional<MovementReason> movementReasonOf(std::string_view text) {
  return valueOfText<MovementReason>(MOVEMENT_REASON_TEXTS, text);
}

std::string_view currencyText(Currency currency) {
  return CURRENCY_TEXTS.at(static_cast<std::size_t>(currency));
}

std::optional<Currency> currencyOf(std::string_view text) {
  return valueOfText<Currency>(CURRENCY_TEXTS, text);
}

namespace {

using MillisecondDate = std::chrono::sys_time<std::chrono::milliseconds>;

std::int64_t millisecondsOf(MillisecondDate date) {
  return date.time_since_epoch().count();
}

MillisecondDate millisecondDateOf(const JsonField& field) {
  return MillisecondDate{
      std::chrono::milliseconds{static_cast<std::int64_t>(integerOf(
          field, IntegerBounds{.lowest = 0, .highest = LATEST_MILLISECONDS}))}};
}

/** @returns The value of @p optional for JSON: null when it holds none. */
template <typename Value>
nlohmann::json nullableOf(const std::optional<Value>& optional) {
  if (!optional.has_value()) {
    return nullptr;
  }
  return *optional;
}

/** @returns The whole number the member @p member of @p message holds within
 * @p bounds, nothing when it is absent or null. */
std::optional<std::uint64_t> nullableIntegerOf(const JsonField& message,
                                               std::string_view member,
                                               IntegerBounds bounds) {
  const std::optional<JsonField> field = optionalMemberOf(message, member);
  if (!field.has_value() || field->value.get().is_null()) {
    return std::nullopt;
  }
  return integerOf(*field, bounds);
}

std::uint64_t roundOf(const JsonField& message) {
  return integerOf(memberOf(message, ROUND_MEMBER),
                   IntegerBounds{.lowest = 0, .highest = HIGHEST_ROUND});
}

/** @returns The chips the member @p member of @p message holds: any whole
 * number of 32 bits. */
std::uint32_t chipsIn(const JsonField& message, std::string_view member) {
  return static_cast<std::uint32_t>(integerOf(
      memberOf(message, member),
      IntegerBounds{.lowest = 0,
                    .highest = std::numeric_limits<std::uint32_t>::max()}));
}

std::optional<std::uint32_t> payoutOf(const JsonField& message) {
  const std::optional<std::uint64_t> payout = nullableIntegerOf(
      message, PAYOUT_MEMBER,
      IntegerBounds{.lowest = 0,
                    .highest = std::numeric_limits<std::uint32_t>::max()});
  if (!payout.has_value()) {
    return std::nullopt;
  }
  return static_cast<std::uint32_t>(*payout);
}

constexpr IntegerBounds WHEEL_NUMBER_BOUNDS{.lowest = 0,
                                            .highest = HIGHEST_ROULETTE_NUMBER};

std::optional<std::uint8_t> winningNumberOf(const JsonField& message) {
  const std::optional<std::uint64_t> number =
      nullableIntegerOf(message, WINNING_NUMBER_MEMBER, WHEEL_NUMBER_BOUNDS);
  if (!number.has_value()) {
    return std::nullopt;
  }
  return static_cast<std::uint8_t>(*number);
}

RouletteBetKind betKindIn(const JsonField& field) {
  const std::optional<RouletteBetKind> kind =
      rouletteBetKindOf(stringOf(field));
  if (!kind.has_value()) {
    refuse(field);
  }
  return *kind;
}

RoulettePhase phaseIn(const JsonField& field) {
  const std::optional<RoulettePhase> phase = roulettePhaseOf(stringOf(field));
  if (!phase.has_value()) {
    refuse(field);
  }
  return *phase;
}

nlohmann::json betDocument(const RouletteBet& bet) {
  nlohmann::json document;
  document[KIND_MEMBER] = rouletteBetKindText(bet.kind);
  document[FIRST_NUMBER_MEMBER] = bet.firstNumber;
  document[AMOUNT_MEMBER] = bet.amount;
  return document;
}

RouletteBet betOf(const JsonField& bet) {
  requireObject(bet);
  return RouletteBet{
      .kind = betKindIn(memberOf(bet, KIND_MEMBER)),
      .firstNumber = static_cast<std::uint8_t>(integerOf(
          memberOf(bet, FIRST_NUMBER_MEMBER),
          IntegerBounds{.lowest = 0,
                        .highest = std::numeric_limits<std::uint8_t>::max()})),
      .amount = static_cast<std::uint32_t>(integerOf(
          memberOf(bet, AMOUNT_MEMBER),
          IntegerBounds{.lowest = 0, .highest = MAXIMUM_READ_BET_AMOUNT}))};
}

nlohmann::json betsDocument(const std::vector<RouletteBet>& bets) {
  nlohmann::json list = nlohmann::json::array();
  for (const RouletteBet& bet : bets) {
    list.push_back(betDocument(bet));
  }
  return list;
}

/** @returns The bets the member bets of @p message lists,
 * MAXIMUM_READ_ROULETTE_BETS at most. */
std::vector<RouletteBet> betsOf(const JsonField& message) {
  const JsonField field = memberOf(message, BETS_MEMBER);
  const std::size_t count = arraySizeOf(field, MAXIMUM_READ_ROULETTE_BETS);
  std::vector<RouletteBet> bets;
  bets.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    bets.push_back(betOf(elementOf(field, index)));
  }
  return bets;
}

std::uint8_t roomIdOf(const JsonField& room) {
  return static_cast<std::uint8_t>(
      integerOf(memberOf(room, ID_MEMBER),
                IntegerBounds{.lowest = FIRST_CASINO_ROOM,
                              .highest = MAXIMUM_CASINO_ROOMS}));
}

std::uint8_t seatsOf(const JsonField& room, std::string_view member) {
  return static_cast<std::uint8_t>(
      integerOf(memberOf(room, member),
                IntegerBounds{.lowest = 0, .highest = MAXIMUM_CASINO_SEATS}));
}

nlohmann::json roomDocument(const CasinoRoom& room) {
  nlohmann::json document;
  document[ID_MEMBER] = room.id;
  document[NAME_MEMBER] = room.name;
  document[SEATS_MEMBER] = room.seats;
  document[SEATED_MEMBER] = room.seated;
  document[ROUND_MEMBER] = room.round;
  document[PHASE_MEMBER] = roulettePhaseText(room.phase);
  return document;
}

CasinoRoom roomOf(const JsonField& room) {
  requireObject(room);
  return CasinoRoom{.id = roomIdOf(room),
                    .name = boundedTextOf(memberOf(room, NAME_MEMBER),
                                          MAXIMUM_CASINO_ROOM_NAME_SIZE),
                    .seats = seatsOf(room, SEATS_MEMBER),
                    .seated = seatsOf(room, SEATED_MEMBER),
                    .round = roundOf(room),
                    .phase = phaseIn(memberOf(room, PHASE_MEMBER))};
}

nlohmann::json casinoPlayerDocument(const CasinoPlayer& player) {
  nlohmann::json document;
  document[NICKNAME_MEMBER] = player.nickname;
  document[STAKE_MEMBER] = player.stake;
  document[PAYOUT_MEMBER] = nullableOf(player.payout);
  return document;
}

CasinoPlayer casinoPlayerOf(const JsonField& player) {
  requireObject(player);
  return CasinoPlayer{.nickname = nicknameOf(memberOf(player, NICKNAME_MEMBER)),
                      .stake = chipsIn(player, STAKE_MEMBER),
                      .payout = payoutOf(player)};
}

std::vector<CasinoPlayer> casinoPlayersOf(const JsonField& state) {
  const JsonField field = memberOf(state, PLAYERS_MEMBER);
  const std::size_t count = arraySizeOf(field, MAXIMUM_CASINO_SEATS);
  std::vector<CasinoPlayer> players;
  players.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    players.push_back(casinoPlayerOf(elementOf(field, index)));
  }
  return players;
}

std::vector<std::uint8_t> historyOf(const JsonField& state) {
  const JsonField field = memberOf(state, HISTORY_MEMBER);
  const std::size_t count = arraySizeOf(field, CASINO_HISTORY_LENGTH);
  std::vector<std::uint8_t> history;
  history.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    history.push_back(static_cast<std::uint8_t>(
        integerOf(elementOf(field, index), WHEEL_NUMBER_BOUNDS)));
  }
  return history;
}

nlohmann::json roomStateDocument(const CasinoRoomState& state) {
  nlohmann::json document;
  document[ID_MEMBER] = state.id;
  document[NAME_MEMBER] = state.name;
  document[SEATS_MEMBER] = state.seats;
  document[ROUND_MEMBER] = state.round;
  document[PHASE_MEMBER] = roulettePhaseText(state.phase);
  document[CLOSES_AT_MEMBER] = millisecondsOf(state.closesAt);
  document[RESULT_AT_MEMBER] = millisecondsOf(state.resultAt);
  document[ENDS_AT_MEMBER] = millisecondsOf(state.endsAt);
  document[SERVER_TIME_MEMBER] = millisecondsOf(state.serverTime);
  document[WINNING_NUMBER_MEMBER] = nullableOf(state.winningNumber);
  document[HISTORY_MEMBER] = state.history;
  nlohmann::json players = nlohmann::json::array();
  for (const CasinoPlayer& player : state.players) {
    players.push_back(casinoPlayerDocument(player));
  }
  document[PLAYERS_MEMBER] = players;
  return document;
}

CasinoRoomState roomStateOf(const JsonField& state) {
  return CasinoRoomState{
      .id = roomIdOf(state),
      .name = boundedTextOf(memberOf(state, NAME_MEMBER),
                            MAXIMUM_CASINO_ROOM_NAME_SIZE),
      .seats = seatsOf(state, SEATS_MEMBER),
      .round = roundOf(state),
      .phase = phaseIn(memberOf(state, PHASE_MEMBER)),
      .closesAt = millisecondDateOf(memberOf(state, CLOSES_AT_MEMBER)),
      .resultAt = millisecondDateOf(memberOf(state, RESULT_AT_MEMBER)),
      .endsAt = millisecondDateOf(memberOf(state, ENDS_AT_MEMBER)),
      .serverTime = millisecondDateOf(memberOf(state, SERVER_TIME_MEMBER)),
      .winningNumber = winningNumberOf(state),
      .history = historyOf(state),
      .players = casinoPlayersOf(state)};
}

}  // namespace

std::string casinoRoomListingText(const CasinoRoomListing& listing) {
  return listingText(ROOMS_MEMBER, listing.rooms, roomDocument);
}

CasinoRoomListing casinoRoomListingFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField field = memberOf(wholeMessage(document), ROOMS_MEMBER);
  const std::size_t count = arraySizeOf(field, MAXIMUM_CASINO_ROOMS);
  CasinoRoomListing listing;
  listing.rooms.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    listing.rooms.push_back(roomOf(elementOf(field, index)));
  }
  return listing;
}

std::string casinoRoomStateText(const CasinoRoomState& state) {
  return textOf(roomStateDocument(state));
}

CasinoRoomState casinoRoomStateFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  return roomStateOf(wholeMessage(document));
}

std::string rouletteBetsText(const RouletteBets& bets) {
  nlohmann::json document;
  document[BETS_MEMBER] = betsDocument(bets.bets);
  return textOf(document);
}

RouletteBets rouletteBetsFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  return RouletteBets{.bets = betsOf(wholeMessage(document))};
}

std::string betsReceiptText(const BetsReceipt& receipt) {
  nlohmann::json document;
  document[ROUND_MEMBER] = receipt.round;
  document[BETS_MEMBER] = betsDocument(receipt.bets);
  document[STAKE_MEMBER] = receipt.stake;
  document[CHIPS_MEMBER] = receipt.chips;
  return textOf(document);
}

BetsReceipt betsReceiptFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField receipt = wholeMessage(document);
  return BetsReceipt{.round = roundOf(receipt),
                     .bets = betsOf(receipt),
                     .stake = chipsIn(receipt, STAKE_MEMBER),
                     .chips = chipsIn(receipt, CHIPS_MEMBER)};
}

std::string roundBetsText(const RoundBets& bets) {
  nlohmann::json document;
  document[ROUND_MEMBER] = bets.round;
  document[BETS_MEMBER] = betsDocument(bets.bets);
  document[STAKE_MEMBER] = bets.stake;
  document[PAYOUT_MEMBER] = nullableOf(bets.payout);
  document[WINNING_NUMBER_MEMBER] = nullableOf(bets.winningNumber);
  return textOf(document);
}

RoundBets roundBetsFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField bets = wholeMessage(document);
  return RoundBets{.round = roundOf(bets),
                   .bets = betsOf(bets),
                   .stake = chipsIn(bets, STAKE_MEMBER),
                   .payout = payoutOf(bets),
                   .winningNumber = winningNumberOf(bets)};
}

std::string_view rouletteBetKindText(RouletteBetKind kind) {
  return ROULETTE_BET_KIND_TEXTS.at(static_cast<std::size_t>(kind));
}

std::optional<RouletteBetKind> rouletteBetKindOf(std::string_view text) {
  return valueOfText<RouletteBetKind>(ROULETTE_BET_KIND_TEXTS, text);
}

std::string_view roulettePhaseText(RoulettePhase phase) {
  return ROULETTE_PHASE_TEXTS.at(static_cast<std::size_t>(phase));
}

std::optional<RoulettePhase> roulettePhaseOf(std::string_view text) {
  return valueOfText<RoulettePhase>(ROULETTE_PHASE_TEXTS, text);
}

}  // namespace rtype::network
