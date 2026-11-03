---
title: "Tutorial: add a network message"
description: A step-by-step tutorial that adds a message to the protocol, from its row on the specification to the handler on the server, with its tests.
---

This tutorial adds a message to the protocol: an **emote**, a salute or a taunt that a player sends to the server. It is small on purpose. It goes through every place a real message touches, in the order you will meet them, starting with the specification, because the code answers to that page and not the other way round. The concepts behind each step are on the [Network](/R-Type/network/#messages-srcnetworkprotocolmessages) page.

Every file below was built and its tests were run on a copy of the repository before this page was written. You need the project to build (see the [README](https://github.com/nique-epi/R-Type#readme)) and a branch of your own started from `origin/main`. `Emote` is an example: do not merge it.

The tutorial ends at the handler on the server. Nothing in the client sends an emote and nothing shows one; [What to change for other cases](#what-to-change-for-other-cases) says where those go.

## What you will have at the end

| Step | You add | In |
|---|---|---|
| 1 | The rule of the message | `docs/src/content/docs/protocol.mdx` |
| 2 | Its type number | `src/network/Protocol/MessageType.hpp` |
| 3 | The struct and its two functions | `src/network/Protocol/Messages/Emote/` |
| 4 | The library of the folder | `Messages/Emote/CMakeLists.txt`, `Messages/CMakeLists.txt` |
| 5 | The handler | `src/server/Sessions/`, `src/server/Sessions/Roster/` |
| 6 | Nine tests | `tests/network/Protocol/`, `tests/server/Sessions/`, `tests/server/Instances/` |
| 7 | The three checks of the project | the terminal |

## Step 0: answer four questions

Decide these on paper first: each answer becomes a sentence of the specification.

| Question | For the emote | Why it matters |
|---|---|---|
| Who sends it? | The client | It decides which table of the page holds it, and who registers a handler |
| Is it a state or a one-time event? | A one-time event, kept as the newest one | Over UDP, "the last received wins" needs an ordering key. The `sequence` of the header is that key: a datagram not newer than the newest one accepted is dropped before any handler runs |
| What happens when it is lost? | Nothing corrects it | A message whose loss the player would notice is repeated (`ReadyState`) or sent three times (`GameEvent`). See [Losses](/R-Type/protocol/#losses) |
| Can a peer of the same version ignore it? | Yes | A receiver skips a type it does not know, so a peer without the message keeps working. If the game cannot run when the other side skips it, `PROTOCOL_VERSION` must change, and the two versions never play together |

Then play the forger: someone who forges an address, sends the message as often as they like, and never reads the answers. An emote from an address that holds no session is dropped before the queues, and one with the wrong token too, so only a real player reaches the handler. That player can still send a thousand a second, so the handler must allocate nothing and write no line. The server never answers an emote, so nothing here sends more bytes than it received.

## Step 1: write the rule

`docs/src/content/docs/protocol.mdx` is the specification: a developer who follows it can write a client without reading the code. Write the message there first, using the words of the page (MUST, SHOULD, MAY as in RFC 2119), so the review of the pull request can compare the code with it.

The next free number is 17 (`ConnectionChallenge` holds 16), and a number never changes once given. Add a row to the table of [Client to server](/R-Type/protocol/#client-to-server):

```md
| 17 | `Emote` | `kind` `u8`: 1 `Salute`, 2 `Taunt` | when the player asks for one, once |
```

To [Payload rules](/R-Type/protocol/#payload-rules), what a receiver must refuse. The page already says that an enumeration value not listed is invalid, so this row only names the field:

```md
| `Emote` | `kind` is in the table of its message |
```

To the table of [Losses](/R-Type/protocol/#losses), what a loss costs:

```md
| `Emote` | not corrected: the emote is missed | — |
```

Finally, [Implementation status](/R-Type/protocol/#implementation-status) must never let a rule read as working when it does not. While the client sends nothing, say so:

```md
- The server keeps the newest `Emote` of each player; the client does not send one yet.
```

## Step 2: give it a number

Add the value to `src/network/Protocol/MessageType.hpp`:

```cpp
  ConnectionChallenge = 16,
  Emote = 17,
```

Two test constants name the first type byte no message holds, and both now point at your message. **Change them by hand: nothing fails if you forget.** The tests stay green, but they stop testing an unknown type.

In `tests/network/Protocol/ProtocolTestConstants.hpp`:

```cpp
constexpr std::uint8_t TYPE_ABOVE_THE_LAST = 18;
```

In `tests/tools/MalformedDatagramConstants.hpp`, where the tool that sends malformed datagrams draws its unknown types, and in the comment above it:

```cpp
constexpr std::uint8_t FIRST_UNKNOWN_MESSAGE_TYPE =
    static_cast<std::uint8_t>(network::MessageType::Emote) + 1;
```

## Step 3: write the struct and its two functions

A message is plain data with a `TYPE`, one function that writes its payload and one that reads it. Create `src/network/Protocol/Messages/Emote/`. `Emote.hpp`:

```cpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/** @brief What a pilot says with an Emote. */
enum class EmoteKind : std::uint8_t {
  Salute = 1,
  Taunt = 2,
};

/**
 * @brief An emote of the player. The client sends it when the player asks for
 * one, once: a lost one is not sent again.
 */
struct Emote {
  static constexpr MessageType TYPE = MessageType::Emote;

  EmoteKind kind;

  bool operator==(const Emote&) const = default;
};

/**
 * @brief Writes the kind as a u8.
 * @throws InvalidFieldValueException when the kind is not listed.
 */
void encodePayload(ByteWriter& writer, const Emote& message);

/**
 * @brief Reads an Emote from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is empty.
 * @throws TrailingBytesException when bytes follow the kind.
 * @throws InvalidFieldValueException when the kind is not listed.
 */
[[nodiscard]] Emote decodePayload(std::span<const std::byte> payload,
                                  std::type_identity<Emote> type);

}  // namespace rtype::network
```

`Emote.cpp`:

```cpp
#include "Emote.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const Emote& message) {
  switch (message.kind) {
    case EmoteKind::Salute:
    case EmoteKind::Taunt:
      return;
  }
  throw InvalidFieldValueException(KIND_FIELD,
                                   static_cast<std::uint8_t>(message.kind));
}

}  // namespace

void encodePayload(ByteWriter& writer, const Emote& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.kind));
}

Emote decodePayload(std::span<const std::byte> payload,
                    [[maybe_unused]] std::type_identity<Emote> type) {
  ByteReader reader{payload};
  const Emote message{.kind = static_cast<EmoteKind>(reader.readUint8())};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
```

| Piece | Why it is written this way |
|---|---|
| `TYPE` | `MessageDispatcher` and `DatagramCodec` read it, so they never list the messages |
| One field at a time | No struct is copied from memory, so neither padding nor the byte order of the machine reaches the wire. `ByteWriter` and `ByteReader` write big-endian |
| `requireFullyRead()` | A byte left after the last field refuses the message: its size must be exact |
| `validate()` on both sides | Writing checks before the first byte, so a refused message leaves the writer untouched. Reading checks what a forger sent |
| `readUint8()` on a short payload | It throws rather than reading out of bounds. `MessageDispatcher` catches every `NetworkException`, so a forged message is dropped and never stops the server |

## Step 4: declare its library

Each message is one folder and one library. `src/network/Protocol/Messages/Emote/CMakeLists.txt`:

```cmake
add_library(rtype_network_emote STATIC Emote.cpp)

target_include_directories(rtype_network_emote PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_emote
  PUBLIC
    rtype_network_protocol
  PRIVATE
    rtype_network_wire_types
    rtype_network_byte_writer
    rtype_network_byte_reader
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_emote)
```

In `src/network/Protocol/Messages/CMakeLists.txt`, add the folder and the library, in alphabetical order:

```cmake
add_subdirectory(Emote)
```

```cmake
    rtype_network_emote
```

## Step 5: write the handler

The code that receives datagrams does not change: a `MessageDispatcher` hands each message to the one handler registered for its type. Register yours where the messages of a player arrive, `GameSessions`, and keep the work in a class of its own, as `ReadyKeeper` does for `ReadyState`.

The player keeps its newest emote. Add the member to `ConnectedPlayer` (`src/server/Sessions/Roster/ConnectedPlayer.hpp`), with `#include "Emote.hpp"`, right after `newestInputNumber`:

```cpp
  /** @brief The newest emote of the player; empty until the first one
   * arrives. EmoteKeeper keeps it. */
  std::optional<network::EmoteKind> lastEmote;
```

`PlayerRoster::add()` builds a `ConnectedPlayer` with designated initializers, in the order of the members. A member with no default must be listed there, or GCC fails the Linux build with `missing initializer for member` while Clang says nothing. In `src/server/Sessions/Roster/PlayerRoster.cpp`:

```cpp
                          .newestInputNumber = std::nullopt,
                          .lastEmote = std::nullopt});
```

`ConnectedPlayer.hpp` now includes `Emote.hpp`, so `rtype_server_roster` must expose it. In `src/server/Sessions/Roster/CMakeLists.txt`, add `rtype_network_emote` to the `PUBLIC` libraries of that target.

Then the class, in `src/server/Sessions/`, beside `InputKeeper`. `EmoteKeeper.hpp`:

```cpp
#pragma once

#include "Emote.hpp"
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Keeps the newest emote of each player.
 *
 * DatagramReader already refused the datagrams that are not newer than the
 * newest accepted from that player, so a late copy never replaces a newer
 * emote. A player can send as many as it likes: apply() allocates nothing and
 * writes no line.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class EmoteKeeper {
 public:
  /** @param roster The players; it must outlive the keeper. */
  explicit EmoteKeeper(PlayerRoster& roster);

  /** @brief Records @p message for the player who sent it, when the sender is
   * a player. */
  void apply(const network::MessageOrigin& origin,
             const network::Emote& message);

 private:
  PlayerRoster& roster_;
};

}  // namespace rtype::server
```

`EmoteKeeper.cpp`:

```cpp
#include "EmoteKeeper.hpp"
#include "ConnectedPlayer.hpp"
#include "Emote.hpp"
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

EmoteKeeper::EmoteKeeper(PlayerRoster& roster) : roster_(roster) {}

void EmoteKeeper::apply(const network::MessageOrigin& origin,
                        const network::Emote& message) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (player == nullptr) {
    return;
  }
  player->lastEmote = message.kind;
}

}  // namespace rtype::server
```

In `src/server/Sessions/CMakeLists.txt`, add the file to the sources of `rtype_server_sessions`, in alphabetical order; the library reaches `rtype_network_emote` through the roster:

```cmake
add_library(rtype_server_sessions STATIC
  DatagramReader.cpp
  Departures.cpp
  EmoteKeeper.cpp
  GameSessions.cpp
```

Finally, plug it in. In `GameSessions.hpp`, include `EmoteKeeper.hpp` and add the member after `readyKeeper_`:

```cpp
  ReadyKeeper readyKeeper_;
  EmoteKeeper emoteKeeper_;
```

In `GameSessions.cpp`, include `Emote.hpp`, build the member, and register the handler in the constructor:

```cpp
      readyKeeper_(roster_, std::move(parts.readyAccepted)),
      emoteKeeper_(roster_),
```

```cpp
  dispatcher_.registerHandler<network::Emote>(
      [this](const network::MessageOrigin& origin,
             const network::Emote& message) {
        emoteKeeper_.apply(origin, message);
      });
```

By the time this handler runs, the checks of [Receiving a datagram](/R-Type/protocol/#receiving-a-datagram) have passed: the sender holds a session, its token is right, its `sequence` is newer than the last one, and the payload was read and checked. The handler only does what the message means.

## Step 6: test it

Each test says what it checks in a Given / When / Then comment. The expected values come from the specification, never from what the code returned: the salute is `0x01` because the row of step 1 says so.

**The message**, in `tests/network/Protocol/EmoteTest.cpp`. The two bounds of the list are tested, as well as a payload too short and one too long:

```cpp
#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ByteSequence.hpp"
#include "Emote.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network {
namespace {

using testing::byteSequence;

/**
 * Given a salute
 * When the Emote is encoded
 * Then the payload is the byte the protocol page gives a salute
 */
TEST(Emote, EncodesASaluteAsOne) {
  const auto expected = byteSequence({0x01});

  EXPECT_EQ(encodeMessage(Emote{.kind = EmoteKind::Salute}), expected);
}

/**
 * Given a taunt
 * When the Emote is encoded then decoded
 * Then the decoded emote is the taunt
 */
TEST(Emote, DecodesWhatItEncoded) {
  const Emote taunt{.kind = EmoteKind::Taunt};

  EXPECT_EQ(decodeMessage<Emote>(encodeMessage(taunt)), taunt);
}

/**
 * Given the byte below the first kind and the byte above the last
 * When each is decoded as an Emote
 * Then an InvalidFieldValueException is raised
 */
TEST(Emote, RefusesAKindOutsideTheList) {
  EXPECT_THROW((void)decodeMessage<Emote>(byteSequence({0x00})),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<Emote>(byteSequence({0x03})),
               InvalidFieldValueException);
}

/**
 * Given an empty payload
 * When it is decoded as an Emote
 * Then a BufferUnderflowException is raised
 */
TEST(Emote, RefusesAnEmptyPayload) {
  const std::vector<std::byte> empty;

  EXPECT_THROW((void)decodeMessage<Emote>(empty), BufferUnderflowException);
}

/**
 * Given a kind followed by one more byte
 * When it is decoded as an Emote
 * Then a TrailingBytesException is raised
 */
TEST(Emote, RefusesBytesAfterTheKind) {
  EXPECT_THROW((void)decodeMessage<Emote>(byteSequence({0x01, 0x00})),
               TrailingBytesException);
}

}  // namespace
}  // namespace rtype::network
```

**The keeper**, in `tests/server/Sessions/EmoteKeeperTest.cpp`, built on the helpers of `SessionsFixtures.hpp`:

```cpp
#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "Emote.hpp"
#include "EmoteKeeper.hpp"
#include "PlayerRoster.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using network::Emote;
using network::EmoteKind;
using testing::addPlayer;
using testing::originOf;
using testing::SOME_SEQUENCE;

/**
 * Given a player who sent no emote
 * When the player sends a salute
 * Then the salute is the emote of the player
 */
TEST(EmoteKeeper, KeepsTheEmoteOfThePlayer) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  EmoteKeeper keeper(roster);

  keeper.apply(originOf(0, SOME_SEQUENCE), Emote{.kind = EmoteKind::Salute});

  EXPECT_EQ(player.lastEmote, EmoteKind::Salute);
}

/**
 * Given a player who sent a salute
 * When the player sends a taunt
 * Then the taunt replaces the salute
 */
TEST(EmoteKeeper, ANewerEmoteReplacesTheOldOne) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  EmoteKeeper keeper(roster);
  keeper.apply(originOf(0, SOME_SEQUENCE), Emote{.kind = EmoteKind::Salute});

  keeper.apply(originOf(0, SOME_SEQUENCE), Emote{.kind = EmoteKind::Taunt});

  EXPECT_EQ(player.lastEmote, EmoteKind::Taunt);
}

/**
 * Given a roster without the sender
 * When an Emote arrives from that sender
 * Then no player has an emote
 */
TEST(EmoteKeeper, IgnoresAnEmoteFromAnAddressNoPlayerSendsFrom) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  EmoteKeeper keeper(roster);

  keeper.apply(originOf(1, SOME_SEQUENCE), Emote{.kind = EmoteKind::Salute});

  EXPECT_FALSE(player.lastEmote.has_value());
}

}  // namespace rtype::server
```

**The handler is registered.** The tests above stay green if `registerHandler` is missing, so one test sends real bytes through a real game. Add it to `tests/server/Instances/GameInstanceTest.cpp`, with `#include "Emote.hpp"`:

```cpp
/**
 * Given a connected player
 * When a datagram holding an Emote arrives from the player
 * Then the game keeps the emote for that player
 */
TEST_F(GameInstanceTest, KeepsTheEmoteAPlayerSends) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  network::PacketHeader header = sessionHeader(FIRST_PLAYER);
  header.sequence = FIRST_SEQUENCE;
  queue(network::IncomingDatagram{
      .sender = playerEndpoint(FIRST_PLAYER),
      .payload = network::DatagramCodec::encode(
          header, network::Emote{.kind = network::EmoteKind::Salute})});

  tick();

  EXPECT_EQ(player.lastEmote, network::EmoteKind::Salute);
}
```

Check that it can fail: remove the `registerHandler<network::Emote>` call, build, and this test turns red; put it back.

Register the files in `tests/CMakeLists.txt`: add `network/Protocol/EmoteTest.cpp` to the `SOURCES` of `network_tests`; add `server/Sessions/EmoteKeeperTest.cpp` to those of `server_sessions_tests`, which links `rtype_server_sessions` already. `GameInstanceTest.cpp` is already listed.

Run them:

```bash
cmake --workflow --preset test
```

The suite ends green with no warning and nine tests more. To run only yours, once the build is done:

```bash
./build/tests/network_tests --gtest_filter='Emote.*'
```

```bash
./build/tests/server_sessions_tests --gtest_filter='EmoteKeeper.*'
```

```bash
./build/tests/server_game_instance_tests --gtest_filter='GameInstanceTest.KeepsTheEmoteAPlayerSends'
```

## Step 7: check the project's gates

Before opening a pull request, the three gates of the project:

```bash
cmake --workflow --preset test
cmake --build build --target format-check
cmake --build build --target tidy
```

A pull request that changes the wire says so in its title, with `!` after the type when a message changes its format (`feat!: …`). The new row of the page goes in the same pull request as the code.

## What to change for other cases

| You want a message that… | Change |
|---|---|
| The client sends | A public function in `ServerConnection` (`src/client/Connection/ServerConnection/`), next to `sendReadyState()`, calling `sendToServer(network::Emote{…})`. `sendToServer` writes the session token and the next sequence |
| The server sends | `send(sessionDatagram(player, network::Emote{…}))`, as `FarewellSender` and `GameFlow` do, so the header carries the token and the next sequence of that player. Add the row to [Server to client](/R-Type/protocol/#server-to-client) |
| The client receives | A `dispatcher_.registerHandler<…>` call in `ServerConnection`, next to the others |
| The players must all see | The server sends it to each player of the game, as `FarewellSender` does. A one-time message goes three times, 50 ms apart (`REPEATED_MESSAGE_COPIES`): see [Losses](/R-Type/protocol/#losses) |
| Must never be lost, or must always reflect the current state | Repeat it until the state message shows it, as `ReadyState` does with `LobbyState`. Guaranteed messages do not exist yet: they are part of [Version 2](/R-Type/protocol/#version-2) |
| Holds a list or a string | Check the count before anything is sized from it, as `LobbyState` does; a client must not make the server allocate in proportion to a value it controls |
| An unknown sender may send | Read [Connection](/R-Type/protocol/#connection) first: `DatagramSorter` drops such a datagram before any queue, unless it holds a `ConnectionRequest`, and an answer must never hold more bytes than the datagram it answers |

## If it does not work

| You see | Cause | Fix |
|---|---|---|
| `'Emote.hpp' file not found` | The library is not linked by the target that includes it | Add it to `Messages/CMakeLists.txt`, or to the libraries of that target |
| `registerHandler` or `decodeMessage` does not compile, naming `DecodableMessage` | `decodePayload()` is missing, or takes another `std::type_identity` | Declare it next to the message, in the namespace of the message |
| `missing initializer for member` on Linux only | A member with no default is left out of a designated initializer | List it at the place that builds the struct, as in step 5 |
| `DuplicateMessageHandlerException` when a game is built | A type has one handler per dispatcher | Remove the second `registerHandler`, and call both from one handler |
| Your handler never runs, and the journal at `--log-level debug` reports a skipped message | `registerHandler` is missing | Add it, as in step 5 |
| Your handler never runs, and the journal at `--log-level debug` reports a malformed one | The payload is refused | Compare the bytes sent with the row of the protocol page |
