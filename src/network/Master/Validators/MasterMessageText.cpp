#include "MasterMessageText.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CreatedServer.hpp"
#include "EndpointText.hpp"
#include "GameListing.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "ListedGame.hpp"
#include "ListedGameState.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"
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

HostedGameState stateOf(const JsonField& field) {
  const std::string text = stringOf(field);
  for (std::size_t index = 0; index < HOSTED_GAME_STATE_TEXTS.size(); ++index) {
    if (text == HOSTED_GAME_STATE_TEXTS.at(index)) {
      return static_cast<HostedGameState>(index);
    }
  }
  refuse(field);
}

std::vector<std::string> playersOf(const JsonField& game) {
  const JsonField players = memberOf(game, PLAYERS_MEMBER);
  const std::size_t count = arraySizeOf(players, MAXIMUM_PLAYERS);
  std::vector<std::string> nicknames;
  nicknames.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    nicknames.push_back(nicknameOf(elementOf(players, index)));
  }
  return nicknames;
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
  return HostedGame{.number = gameNumberOf(game),
                    .name = gameNameOf(game),
                    .state = stateOf(memberOf(game, STATE_MEMBER)),
                    .difficulty = nicknameOf(memberOf(game, DIFFICULTY_MEMBER)),
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
  const std::string text = stringOf(field);
  for (std::size_t index = 0; index < LISTED_GAME_STATE_TEXTS.size(); ++index) {
    if (text == LISTED_GAME_STATE_TEXTS.at(index)) {
      return static_cast<ListedGameState>(index);
    }
  }
  refuse(field);
}

ListedGame listedGameOf(const JsonField& game) {
  requireObject(game);
  return ListedGame{.number = gameNumberOf(game),
                    .name = gameNameOf(game),
                    .state = listedStateOf(memberOf(game, STATE_MEMBER)),
                    .playerCount = static_cast<std::uint8_t>(integerOf(
                        memberOf(game, PLAYER_COUNT_MEMBER),
                        IntegerBounds{.lowest = MINIMUM_LISTED_PLAYERS,
                                      .highest = MAXIMUM_PLAYERS}))};
}

nlohmann::json listedGameDocument(const ListedGame& game) {
  nlohmann::json document;
  document[NUMBER_MEMBER] = game.number;
  document[NAME_MEMBER] = game.name;
  document[STATE_MEMBER] =
      LISTED_GAME_STATE_TEXTS.at(static_cast<std::size_t>(game.state));
  document[PLAYER_COUNT_MEMBER] = game.playerCount;
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
          measureOf(measures, AVERAGE_PING_MEMBER, MAXIMUM_MEASURE)};
}

nlohmann::json gameDocument(const HostedGame& game) {
  nlohmann::json document;
  document[NUMBER_MEMBER] = game.number;
  document[NAME_MEMBER] = game.name;
  document[STATE_MEMBER] =
      HOSTED_GAME_STATE_TEXTS.at(static_cast<std::size_t>(game.state));
  document[DIFFICULTY_MEMBER] = game.difficulty;
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
  return document;
}

std::string serverKeyOf(const JsonField& field) {
  std::string key = stringOf(field);
  if (!isWellFormedServerKey(key)) {
    refuse(field);
  }
  return key;
}

}  // namespace

bool isWellFormedServerKey(std::string_view text) {
  return text.size() == SERVER_KEY_SIZE &&
         text.starts_with(SERVER_KEY_PREFIX) &&
         std::ranges::all_of(
             text.substr(SERVER_KEY_PREFIX.size()), [](char digit) {
               return std::ranges::find(SERVER_KEY_DIGITS, digit) !=
                      SERVER_KEY_DIGITS.end();
             });
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
  return textOf(document);
}

ServerRegistration registrationFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField registration = wholeMessage(document);
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

std::string heartbeatText(const ServerHeartbeat& heartbeat) {
  nlohmann::json document;
  document[GAMES_MEMBER] = nlohmann::json::array();
  for (const HostedGame& game : heartbeat.games) {
    document[GAMES_MEMBER].push_back(gameDocument(game));
  }
  document[MEASURES_MEMBER] = measuresDocument(heartbeat.measures);
  return textOf(document);
}

ServerHeartbeat heartbeatFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField heartbeat = wholeMessage(document);
  return ServerHeartbeat{.games = gamesOf(heartbeat),
                         .measures = measuresOf(heartbeat)};
}

std::string gameListingText(const GameListing& listing) {
  nlohmann::json document;
  document[GAMES_MEMBER] = nlohmann::json::array();
  for (const ListedGame& game : listing.games) {
    document[GAMES_MEMBER].push_back(listedGameDocument(game));
  }
  return textOf(document);
}

GameListing gameListingFromText(std::string_view text) {
  const nlohmann::json document = parsedObject(text);
  const JsonField games = memberOf(wholeMessage(document), GAMES_MEMBER);
  const std::size_t count = arraySizeOf(games, MAXIMUM_HOSTED_GAMES);
  GameListing listing;
  listing.games.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    listing.games.push_back(listedGameOf(elementOf(games, index)));
  }
  requireDistinctNumbers(games, listing.games);
  return listing;
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

}  // namespace rtype::network
