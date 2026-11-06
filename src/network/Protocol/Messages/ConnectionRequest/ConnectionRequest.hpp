#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief Which game a player asks a supervisor for. The values are the ones of
 * the protocol page; any other byte is invalid.
 */
enum class GameChoiceKind : std::uint8_t {
  /** @brief Any game that has a free seat, or a new one when none has. */
  AnyGame = 1,
  /** @brief The game whose number GameChoice::number names. */
  ExistingGame = 2,
  /** @brief A game of its own, which other players may join. */
  NewGame = 3,
};

/**
 * @brief The game a ConnectionRequest asks for: its kind, and for
 * ExistingGame the number of the game on the server, FIRST_HOSTED_GAME_NUMBER
 * or more, as the master lists it; UNUSED_GAME_NUMBER for the other kinds.
 */
struct GameChoice {
  GameChoiceKind kind{GameChoiceKind::AnyGame};
  std::uint16_t number{UNUSED_GAME_NUMBER};

  bool operator==(const GameChoice&) const = default;
};

/** @brief The choice of a player who takes whatever game has a seat. */
constexpr GameChoice ANY_GAME_CHOICE{.kind = GameChoiceKind::AnyGame,
                                     .number = UNUSED_GAME_NUMBER};

/** @brief The choice of a player who opens a game of its own. */
constexpr GameChoice NEW_GAME_CHOICE{.kind = GameChoiceKind::NewGame,
                                     .number = UNUSED_GAME_NUMBER};

/**
 * @brief What a client sends to join a game, every 500 ms until it gets an
 * answer: the cookie of the server's challenge, 0 until it has one, the
 * nickname of the player and the game it asks for. A game process reads the
 * choice and ignores it: the supervisor already chose its game.
 */
struct ConnectionRequest {
  static constexpr MessageType TYPE = MessageType::ConnectionRequest;

  std::uint64_t cookie;
  std::string nickname;
  GameChoice game{ANY_GAME_CHOICE};

  bool operator==(const ConnectionRequest&) const = default;
};

/**
 * @brief Writes the cookie, the nickname with its length, then the kind and
 * the number of the game.
 * @throws InvalidNicknameException when the nickname is empty, longer than
 * MAXIMUM_NICKNAME_SIZE or not UTF-8; nothing is written then.
 * @throws InvalidFieldValueException when the kind is not one of
 * GameChoiceKind, or the number does not fit it (see GameChoice); nothing is
 * written then.
 */
void encodePayload(ByteWriter& writer, const ConnectionRequest& message);

/**
 * @brief Reads a ConnectionRequest from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidNicknameException when the nickname is empty, longer than
 * MAXIMUM_NICKNAME_SIZE or not UTF-8.
 * @throws InvalidFieldValueException when the kind or the number of the game
 * is invalid, as for encodePayload().
 * @throws TrailingBytesException when bytes follow the number of the game.
 */
[[nodiscard]] ConnectionRequest decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionRequest> type);

}  // namespace rtype::network
