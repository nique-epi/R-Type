---
title: Network
description: What the network library does, how its parts fit together and where to find them in the code.
---

The network library, `rtype_network`, carries datagrams between the programs. Today it opens UDP sockets, sends datagrams and hands over the ones it receives, with their sender, through queues when another thread uses them. It knows nothing about entities or the rules of the game, and it is the only library that includes Asio: the client and the server use it through classes that never name an Asio type (see [Project layout](/R-Type/project-layout/#network)). It also turns every message of the protocol into bytes and back, one field at a time, and hands each message of a received datagram to the code registered for its type. What the datagrams contain is described in [Protocol](/R-Type/protocol/).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Event loop | Available | `src/network/NetworkContext/` |
| UDP socket | Available | `src/network/Transport/` |
| Ports and addresses written as text | Available | `src/network/Transport/EndpointText.hpp` |
| Relay between a socket and the queues of another thread | Available | `src/network/Transport/DatagramRelay/` |
| Network simulator: lag, jitter, loss, duplicates and reordering, off by default | Available | `src/network/Transport/Simulator/`, `src/network/Transport/Conditions/` |
| Serialization | Available | `src/network/Serialization/` |
| Protocol messages, datagram header, dispatch by type | Available | `src/network/Protocol/` |
| Connection cookies | Available | `src/network/Handshake/` |
| Rate limit of the requests, sessions, reliability, history of states, statistics | Not yet written | — |
| Test tool that sends malformed datagrams | Available, never shipped | `tests/tools/` |

## Event loop: `NetworkContext`

`NetworkContext` owns the Asio event loop. `run()` blocks and runs every network operation, and every socket handler, on the thread that called it. It returns when the loop has no work left or `stop()` is called; a socket that is receiving always has work, so a server returns only on `stop()`.

`stop()` and `post(task)` may be called from any thread. `post()` queues a function to run on the thread that runs `run()`: this is how another thread gets a datagram sent, since a socket may only be used on that thread. A task still queued when the context is destroyed is destroyed without running.

Asio is linked privately to the network targets, so the headers other modules include never include it, and `r-type_server` fails to build if it tries. A class that needs Asio lives in `src/network` and links `rtype_network_asio`, which also defines `ASIO_NO_DEPRECATED` and, on Windows, `_WIN32_WINNT`.

## Sockets: `UdpSocket`

`UdpSocket` (`src/network/Transport/UdpSocket/`) implements `IDatagramSocket`. It opens a UDP socket on an `Endpoint`, sends datagrams and hands every datagram received to a handler. None of its calls blocks. Code that only sends and receives can take an `IDatagramSocket&`, so the [network simulator](#network-simulator-simulatednetworksocket) or a test double stands in for the real socket.

- An `Endpoint` is an IP address written as text, `"127.0.0.1"` or `"::1"`, and a port. Host names are never resolved: `"localhost"` throws `InvalidAddressException`. Port 0 (`ANY_PORT`) lets the system pick a free port, which `localEndpoint()` returns. A port already taken throws `SocketOpenException`.
- `startReceiving(handler)` is called once; a second call throws `ReceivingAlreadyStartedException`. The handler gets the sender and a `std::span` of the bytes, valid only during the call. It must not throw: an exception leaves `run()` and the socket stops receiving.
- A datagram larger than `MAX_DATAGRAM_SIZE` (1200 bytes) never reaches the handler; it is logged at the `debug` level with its sender, at most once per `REJECTION_LINE_INTERVAL` (10 s), with how many since the previous line (see [Logging](/R-Type/logging/#repeated-events-throttledlog)). The socket receives into `RECEIVE_BUFFER_SIZE` (1201) bytes, so a datagram cut down to the buffer by the system is told apart from one of exactly 1200 bytes. An empty datagram arrives as an empty span: checking sizes is the job of the code that decodes it.
- `send(destination, payload)` copies the payload and returns at once. More than `MAX_DATAGRAM_SIZE` bytes throws `DatagramTooLargeException` and sends nothing. A failed send is logged, and UDP never tells whether a datagram arrived.
- An error while receiving is logged and the socket keeps receiving. On Windows, a datagram sent to a port nobody listens on makes a later receive fail with "connection refused"; it is logged at the `debug` level.
- A socket is not thread-safe: use it and destroy it on the thread that runs its `NetworkContext`. Once it is destroyed, its handler is never called again, even for a datagram already received.
- `IncomingDatagram` and `OutgoingDatagram` (`src/network/Transport/`) hold a copy of a datagram's bytes with its sender, or with its destination, for code that keeps a datagram after the handler returns or hands it to another thread.

An echo server, which sends every datagram back to its sender:

```cpp
rtype::network::NetworkContext network;
rtype::network::UdpSocket socket{
    network, rtype::network::Endpoint{
                 .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
                 .port = rtype::network::DEFAULT_SERVER_PORT}};
socket.startReceiving([&socket](const rtype::network::Endpoint& sender,
                                std::span<const std::byte> payload) {
  socket.send(sender, payload);
});
network.run();
```

`r-type_server` opens such a socket on the port given at launch, `DEFAULT_SERVER_PORT` (4242) by default, on every IPv4 interface, sorts each datagram on its network thread, and hands the datagrams of admitted players and the connection requests to its simulation thread, which answers the requests, reads the `ReadyState` and the `PlayerInput` of the players and nothing else yet (see [Server](/R-Type/server/#sorting-the-datagrams-datagramsorter)). The client runs its socket on a thread of its own, `NetworkThread`, described in [Client](/R-Type/client/#network-networkthread).

## Ports and addresses written as text: `portFromText`, `ipv4AddressFromText`

`EndpointText.hpp` (`src/network/Transport/`) turns what a person types into the parts of an `Endpoint`, with the same rule on every system. The client and the server read their launch options with it.

- `portFromText()` accepts decimal digits only, from `LOWEST_PORT` (1) to `HIGHEST_PORT` (65535): no sign, no space, nothing after the digits. Port 0, which asks the system for any free port, is refused: the players could not know it. Anything else throws `InvalidPortException`.
- `ipv4AddressFromText()` accepts four decimal numbers from 0 to 255 separated by dots, such as `LOOPBACK_IPV4_ADDRESS` (`127.0.0.1`), and returns the text unchanged. A number may not start with 0 unless it is 0. A host name, an IPv6 address or anything else throws `InvalidIpv4AddressException`.

They do not ask Asio, which hands the text to the system: `inet_pton` on Linux and macOS, `WSAStringToAddress` on Windows, whose documentation lets the text carry a port. On macOS, `inet_pton` reads `01` as 1 while `inet_aton` reads a leading 0 as octal, so a text with one could name two addresses.

## Socket to queues: `DatagramRelay`

`DatagramRelay` (`src/network/Transport/DatagramRelay/`) connects an `IDatagramSocket` to two `BoundedQueue` (see [Concurrency](/R-Type/concurrency/)), so another thread exchanges datagrams with the network without touching the socket. Its calls run on the thread that runs the socket's `NetworkContext`, or before that thread starts. The socket and the outgoing queue must outlive it. The incoming queue must live longer still: `startReceiving()` gives the socket a handler that writes to it, and the socket keeps that handler until it is destroyed, so the incoming queue stays alive until the socket is destroyed, or until no thread will run its `NetworkContext` again. `ServerApplication` only sends through it: its socket hands what it receives to the server's `DatagramSorter` instead (see [Server](/R-Type/server/#sorting-the-datagrams-datagramsorter)). The client's `NetworkThread` uses both directions and respects the rule: its destructor stops the context and joins the thread before its queues are destroyed.

- `startReceiving()` copies every datagram the socket receives, with its sender, into the incoming queue as an `IncomingDatagram`.
- `sendWaiting()` sends every `OutgoingDatagram` waiting in the outgoing queue, oldest first. A datagram the socket refuses, too large or for an address that is not an IP address, throws, and the ones after it are not sent.
- The other thread drains the incoming queue and pushes to the outgoing one; to get what it pushed sent, it hands `sendWaiting()` to the network thread with `NetworkContext::post()`.

The server only sends through one: it receives through its own `DatagramSorter`, which splits the datagrams between two queues (see [Server](/R-Type/server/#sorting-the-datagrams-datagramsorter)).

## Network simulator: `SimulatedNetworkSocket`

Either program can make its datagrams cross a simulated network, to show on one machine what a poor connection does. `openDatagramSocket()` (`src/network/Transport/Simulator/`) opens the socket of the server (`ServerApplication::open()`) and of the client (`NetworkThread::open()`). For `PERFECT_NETWORK`, every value at zero, it returns the `UdpSocket` itself, so a program launched without the options below pays nothing. Otherwise it returns a `SimulatedNetworkSocket` over the `UdpSocket` and logs a warning such as `network simulated: ping +300 ms, up to 50 ms more, 10 % lost, 2 % duplicated, 5 % reordered`, so a journal tells whether a measurement was taken on a simulated network.

`NetworkConditions` (`src/network/Transport/Conditions/`) holds what the ping shows. Every datagram the program sends or receives crosses the simulated network once, so a client alone given `--lag 100` sees its ping grow by 100 ms, and a server given options adds its own:

- it waits half the lag, then up to half the jitter more, drawn at random, but never arrives before the datagram handed over before it: the jitter bunches datagrams and never reorders them;
- a share of the datagrams never arrives, another share arrives twice, and a lost datagram is never copied;
- a share is held `REORDER_HOLD` (75 ms) longer, so the ones handed over just after it arrive first.

The draws start from fixed seeds (`SENT_DATAGRAMS_SEED`, `RECEIVED_DATAGRAMS_SEED`), so two runs that hand over the same datagrams at the same times lose the same ones. A datagram the real socket would refuse is refused at once, as `IDatagramSocket::send()` promises, and one still in flight when the socket is destroyed never arrives.

Both programs read these options with `SimulatorLaunchOptions` before their own, in the order given, so an option written after `--network` changes one value of its scenario. As with the logging options, a value of another option spelled like one of them, a nickname `--lag` for example, is read as that option.

| Option | Effect |
|---|---|
| `--network <name>` | The five values of a scenario of `NETWORK_SCENARIOS`, below. |
| `--lag <ms>` | What the ping gains, from 0 to `MAXIMUM_SIMULATED_DELAY` (10000). |
| `--jitter <ms>` | How much the ping may vary, from 0 to 10000. |
| `--loss <percent>` | The share of the datagrams lost, from 0 to `MAXIMUM_PERCENT` (100). |
| `--duplicates <percent>` | The share of the datagrams that arrive twice, from 0 to 100. |
| `--reorder <percent>` | The share of the datagrams held longer, from 0 to 100. |

| Scenario | Ping | Jitter | Lost | Duplicated | Reordered |
|---|---|---|---|---|---|
| `4g` | +60 ms | 20 ms | 1 % | 0 % | 0 % |
| `wifi-busy` | +30 ms | 60 ms | 3 % | 1 % | 2 % |
| `bad` | +300 ms | 50 ms | 10 % | 2 % | 5 % |

The values of the scenarios were chosen for the demonstrations, not measured on real networks.

## Serialization: `ByteWriter` and `ByteReader`

`ByteWriter` appends values to a `std::vector<std::byte>` and `ByteReader` reads them back from a buffer. Every value is written field by field, never copied from a struct in memory, so neither padding nor the byte order of the machine reaches the wire.

| Call | On the wire |
|---|---|
| `writeUint8` to `writeUint64`, `writeInt8` to `writeInt64` | 1, 2, 4 or 8 bytes, big-endian; signed values in two's complement |
| `writeFloat` | 4 bytes, IEEE-754 binary32, big-endian |
| `writeString` | one length byte, then the characters, at most `MAX_SHORT_STRING_LENGTH` (255) |
| `writeBytes` | the bytes as they are, without a length |

The build fails on a platform where `float` is not binary32. Version 1 of the protocol sends no float (positions are quantized integers), so `writeFloat` is there for later use.

`ByteReader` does not own its buffer: the caller keeps the bytes alive while it reads, and `readBytes(count)` returns a view into it, not a copy. Every read first compares what it needs with `remaining()`. A read that needs more throws `BufferUnderflowException`, and the position does not move, so a truncated or forged datagram is rejected without reading outside the buffer. A string announcing more characters than follow is rejected the same way, before anything is allocated.

`writeString` throws `StringTooLongException` for more than 255 bytes and appends nothing. It copies the bytes as they are and the reader does not check that they are UTF-8, so a caller that needs valid text must check it, as the messages below do for nicknames.

Both exceptions derive from `NetworkException`, the root of every error of this library, so one `catch` handles them all.

```cpp
rtype::network::ByteWriter writer;
writer.writeUint16(sequence);
writer.writeString(playerName);

rtype::network::ByteReader reader(writer.bytes());
const auto decodedSequence = reader.readUint16();
const auto decodedName = reader.readString();
```

`WireTypes` (`src/network/Protocol/`) builds the `bool`, the position and the nickname of the protocol on top of these calls: a `bool` other than 0 or 1 throws `InvalidBoolException`, a coordinate that is NaN throws `InvalidCoordinateException` before anything is written, and `isValidNickname` accepts 1 to 16 bytes of UTF-8 as RFC 3629 defines it, with no control character: no byte below `SPACE_BYTE` (0x20) and no `DELETE_BYTE` (0x7F), so a nickname written in a log line cannot start a line of its own. In UTF-8 each of these characters is one byte of that value, and every byte of a longer sequence is 0x80 or above, so checking the bytes finds exactly them.

## Datagrams: `DatagramCodec`

`DatagramCodec` (`src/network/Protocol/DatagramCodec/`) turns a header and one message into the bytes of a datagram, and a received datagram back into its header and its messages; the layout is in [Protocol](/R-Type/protocol/#datagram).

- `encode(header, message)` writes the `PACKET_HEADER_SIZE` (22) bytes of the `PacketHeader`, the type of the message, the size of its payload on two bytes, then the payload. A payload over `MESSAGE_PAYLOAD_LIMIT` (1175 bytes) throws `DatagramTooLargeException`. A default `PacketHeader` is version 1 without a session; the sender sets `sessionToken` and `sequence`.
- `decodeHeader(datagram)` reads the header without judging it, so a datagram of another version still gives its version. Fewer than 22 bytes throws `MalformedDatagramException`.
- `splitMessages(datagram)` returns the `MessageFrame`s that follow the header: the type byte as it arrived, and a view of the payload into the datagram. Either the sizes add up exactly to the end of the datagram, or it throws `MalformedDatagramException` and returns nothing: a datagram without a message, with a size running past its end, or with bytes after its last message is refused whole. Calling it on a temporary buffer does not compile.

Reading the header and splitting the messages are two calls because a receiver checks the version before it trusts the rest. `speaksThisProtocol(header)` is that check: `protocolId`, `version` and `flags` are those of this code. The server's `ConnectionAcceptor` and `GameInstance` and the client's `ServerConnection` call it before splitting. Of the checks of [Receiving a datagram](/R-Type/protocol/#receiving-a-datagram) that follow, the server's `DatagramSorter` checks the session token of every datagram from the address of an admitted player (see [Server](/R-Type/server/#sorting-the-datagrams-datagramsorter)) and its `DatagramReader` the sequence of every datagram of a player (see [Server](/R-Type/server/#order-and-loss-arrivalledger)); the client's `ServerConnection`, once accepted, checks the session token and drops a datagram whose `sequence` is not newer than the newest one it accepted (`isNewerSequence`).

## Messages: `src/network/Protocol/Messages/`

Each message of the protocol page is a struct of the same name (`ConnectionRequest`, `ReadyState`, `PlayerInput`…), plain data that holds its `MessageType` in `TYPE`. Next to it, `encodePayload(writer, message)` writes its payload, and `decodeMessage<Message>(payload)` reads one. The fifteen messages other than `WorldState` live in `Messages/`, one folder and one library each (`Messages/LobbyState/`, target `rtype_network_lobby_state`), gathered by `rtype_network_messages`; `WorldState` keeps its codec (below), and `decodeMessage<WorldState>` reads one of its payloads.

- Every value of these fifteen messages is checked as the protocol page lists it: a `bool` is 0 or 1, an enumeration holds a listed value, a player number is 1 to 4 and appears once in a list, a nickname is 1 to 16 bytes of UTF-8 without a control character, no action bit above the five actions is set, no entity id is 0, and the player or the entity a `GameEvent` names matches its kind. A count is checked before anything is sized from it, and a byte left after the last field refuses the message.
- Writing runs the same checks before the first byte, so a refused message leaves the writer untouched, and whatever was read can be written back: a nickname accepted from a `ConnectionRequest` never makes a `LobbyState` throw.
- Reading a refused message throws an exception derived from `MalformedMessageException`, or `BufferUnderflowException` for a payload cut short. Writing one throws the same exceptions, except for a coordinate that is NaN, which throws `InvalidCoordinateException`; `WorldStateSender` leaves such an entity out before encoding (see [Server](/R-Type/server/#world-state-worldstatesender)).

Adding a message takes a folder of `Messages/` holding its struct, with its `TYPE` and its two functions, and its library, listed in `Messages/CMakeLists.txt`; then its value in `MessageType` and its row on the protocol page. Nothing that receives datagrams changes.

```cpp
rtype::network::PacketHeader header;
header.sessionToken = sessionToken;
header.sequence = nextSequence;

const std::vector<std::byte> datagram = rtype::network::DatagramCodec::encode(
    header, rtype::network::ReadyState{.ready = true});
```

## Dispatch: `MessageDispatcher`

`MessageDispatcher` (`src/network/Protocol/MessageDispatcher/`) hands each received message to the handler registered for its type, with a `MessageOrigin`: the sender, and the header of the datagram, which carries the session token a client reads from `ConnectionAccepted`.

- `registerHandler<ReadyState>(handler)` registers the one handler of a type; a second one throws `DuplicateMessageHandlerException`.
- `dispatch(origin, messages)` reads each payload, then calls its handler, in order. A message whose type has no handler, which includes the type bytes no message has, is skipped; a message whose payload is refused is dropped. Both are counted (`skippedMessageCount()`, `malformedMessageCount()`) and logged at the `debug` level with the sender and why, at most once per `REJECTION_LINE_INTERVAL` (10 s), with how many since the previous line (see [Logging](/R-Type/logging/#repeated-events-throttledlog)); the next message is handled.
- No exception caused by the content of a message leaves `dispatch()`, so one forged message cannot stop the server. An exception thrown by a handler does: it is a fault of the program.
- A sender it does not know yet must be answered at most once per datagram: ninety 13-byte `ConnectionRequest` fit in one datagram, and an answer to each would send more bytes than were received. A handler cannot tell which messages shared a datagram, so the code that receives must enforce it before dispatching.
- It is not thread-safe. The client's `ServerConnection` hands it the datagrams of the server it contacts, and each server `GameInstance` the datagrams of its players; the server's `ConnectionAcceptor` does not use it, since it reads the first `ConnectionRequest` of each datagram itself, which keeps it to one answer per datagram.

```cpp
rtype::network::MessageDispatcher dispatcher;
dispatcher.registerHandler<rtype::network::ReadyState>(
    [&lobby](const rtype::network::MessageOrigin& origin,
             const rtype::network::ReadyState& message) {
      lobby.setReady(origin.sender, message.ready);
    });

dispatcher.dispatch(
    {.sender = received.sender,
     .header = rtype::network::DatagramCodec::decodeHeader(received.payload)},
    rtype::network::DatagramCodec::splitMessages(received.payload));
```

`lobby` stands for the code that will handle the waiting room; `received` is an `IncomingDatagram`. `decodeHeader` and `splitMessages` throw `MalformedDatagramException` for bytes that are not a header followed by whole messages: the code that receives must catch it and drop the datagram, or one forged datagram would stop the server. `messageTypes(datagram)` runs the checks of `splitMessages` without throwing or allocating, and returns the set of message types the datagram holds, or nothing: the server sorts every datagram it receives with it, since throwing and catching an exception costs about 4 µs, a hundred times the cost of sorting a datagram.

## Connection cookies: `CookieIssuer`

`CookieIssuer` (`rtype_network_cookie_issuer`, `src/network/Handshake/CookieIssuer/`) issues the cookie of a `ConnectionChallenge` and recognizes it when the sender sends it back, so the server gives a place only to the owner of an address and keeps nothing per request (see [Protocol](/R-Type/protocol/#connection)).

- `issue(sender, now)` is the SipHash-2-4 (`sipHash()`, `src/network/Handshake/SipHash.hpp`), under a secret key, of the sender's address, its port and the number of the cookie window: the time divided by `COOKIE_WINDOW_DURATION` (10 s). Without the key, nobody can compute it.
- `isValid(cookie, sender, now)` accepts the cookie of that sender in the window of `now` and in the one before it, so a cookie stays valid at least 10 s and less than 20 s after it was issued, depending on when in its window that was, and a challenge answered just before a window ends still works. 0, the cookie of a request that has none yet, is never valid.
- The default constructor draws the key from `std::random_device`; another constructor takes the key, for tests. `now` is any monotonic time, the same origin for every call.

`sipHash()` follows the reference implementation of Aumasson and Bernstein, and its tests check the reference vectors. The window duration is **provisional**, like every delay of the protocol page: code must read `COOKIE_WINDOW_DURATION` (`HandshakeConstants.hpp`).

## World state: `WorldStateCodec`

A `WorldState` (`src/network/Protocol/WorldState.hpp`) is the picture of the game at one server tick: the `tick`, the players, and every entity with its network id, a `WireEntityType` and its position in logical units. It is plain data and knows nothing of the game's components, so the server and the client each translate to and from their own. `WorldStateCodec` (`src/network/Protocol/WorldStateCodec/`) turns it into the payloads of the `WorldState` message and back; the byte layout is in [Protocol](/R-Type/protocol/#world-state).

- `encode(state)` returns one payload per datagram. A payload holds at most 1175 bytes, the 1200 of `MAX_DATAGRAM_SIZE` less the 22-byte packet header and the 3-byte message prefix (`PACKET_HEADER_SIZE`, `MESSAGE_PREFIX_SIZE`), which is 124 entities next to 4 players. A state with more entities gives several payloads with the same tick, each with every player and a part of the entities, in order; a state without entities gives one payload. More than `MAXIMUM_PLAYERS` (4) players throws `TooManyPlayersException`.
- A position is rounded to the nearest eighth of a unit and clamped to -4096 to 4095.875 units, the range of a wire coordinate. The clamp is a last resort, so a coordinate never wraps to the opposite edge; the server must still destroy an entity before it leaves the range (see [Protocol](/R-Type/protocol/#positions)).
- `decode(payload)` throws `BufferUnderflowException` for a truncated payload, and `MalformedMessageException` for more than 4 players, a player number outside 1 to 4 or listed twice, a boolean other than 0 and 1, an entity of id 0, or bytes after the last entity. An entity of a type outside 1 to 4 is left out of the state and the others are decoded, so a server with a new kind of entity does not blind an older client. Whatever a payload announces, it makes the receiver allocate for at most 4 players and 255 entities. It accepts 0 players, where the protocol page says 1 to 4: the server sends none until the game has players.
- The server puts each payload in its own datagram with `DatagramCodec::encode(header, MessageType::WorldState, payload)`, which adds the packet header and the message prefix; the header carries the session token and the next sequence of the player it goes to (see [Server](/R-Type/server/#session-headers-sessiondatagram)).

```cpp
for (const std::vector<std::byte>& payload :
     rtype::network::WorldStateCodec::encode(state)) {
  send(recipient, payload);
}

const rtype::network::WorldState received =
    rtype::network::WorldStateCodec::decode(payload);
```

## Malformed datagrams: `malformed_datagrams`

Anyone can send a datagram to the server, and a client reads whatever comes from the address of its server, so both must survive bytes that are not a valid message. The unit tests check each reader on its own; `malformed_datagrams` checks the programs as they run. It is a test tool in `tests/tools/`, built with the `test` preset into `build/tests/` and never shipped, and it only talks to the loopback address, 127.0.0.1.

For `FLOOD_DURATION` (60 s) it sends `FLOOD_DATAGRAMS_PER_SECOND` (1000) datagrams per second, seven kinds in turn, written by `MalformedDatagramBuilder` from a fixed seed (`FLOOD_SEED`), so a run can be replayed. Every kind but oversized and random bytes starts from a valid message of the protocol and breaks it in one way:

| Kind | What the datagram holds | Where a receiver stops it |
|---|---|---|
| Truncated | a valid message, cut before its last byte | splitting the messages |
| Too long | a valid message followed by random bytes, counted in its size | the reader of its type |
| Oversized | 1201 to 2400 random bytes | the socket |
| Unknown type | a message of type 17 to 255, holding up to 64 random bytes | no reader: skipped |
| Lying size | a valid message whose size announces more bytes than follow it | splitting the messages |
| Extreme value | a valid message whose bounded one-byte field (a bool, a player number, a reason, a count of players) holds 255 | the reader of its type |
| Random bytes | up to 1200 random bytes | almost always the header |

The tests of the tool (`malformed_datagram_tests`) check that each kind is stopped where the table says, so a flood never sends a valid message by mistake.

```bash
./build/tests/malformed_datagrams --server 4242
```

This joins the server listening on port 4242 as a player, so the datagrams carry a session token and reach the readers of the game instead of stopping at the sorting of the network thread. After the flood, it checks that the server still answers a connection request: the exit code is 0 if it does, 1 otherwise. `--client 4243` waits on port 4243 for a client launched with `-h 127.0.0.1 -p 4243`, accepts it as player 1, floods it, then checks that the client still sends a datagram in its session: the exit code is 0 if it does, 1 otherwise. At 1000 per second the flood stays below what the player queue of the server takes (256 datagrams per tick, 60 ticks per second), so it tests the readers, not the queue. A player flooding with its own token can still push the datagrams of the others out of that shared queue (see [Flood protection](/R-Type/server/#flood-protection)).

Measured on 27 October 2026 on an Apple M4 Pro under macOS 27.0, with the server built in Release: the tool sent 59,999 datagrams in the minute, none refused by the system, and received 121 datagrams from the server meanwhile; the server only sends to the players of its game, so the session of the tool stayed open. The resident memory of the server was 2,576 KiB before the flood. It rose to 3,264 KiB during the first 25 seconds, then stayed at 3,264 KiB until 10 seconds after the flood. The server still answered a connection request, and its log held 48 lines, at most one per kind of rejection every 10 seconds. The client, built in Release and accepted by `--client` on the same machine, stayed connected for the whole minute and kept sending the tool about one datagram per second (59). Its resident memory rose from 101,120 KiB to 103,552 KiB in the first 15 seconds of the flood, then by about 16 KiB every 5 seconds. Without the flood, in the waiting room of a real server, it rises at the same pace (102,688 to 103,136 KiB over 65 seconds), so the flood adds no growth of its own after its first seconds.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change the largest datagram | `MAX_DATAGRAM_SIZE` in `src/network/NetworkConstants.hpp`, and the Transport section of [Protocol](/R-Type/protocol/#transport) |
| Change the port the server listens on by default | `DEFAULT_SERVER_PORT` in `src/network/NetworkConstants.hpp` |
| Change which ports or addresses a program accepts at launch | `portFromText()` and `ipv4AddressFromText()` in `src/network/Transport/EndpointText.cpp`, `LOWEST_PORT` in `NetworkConstants.hpp` |
| Change how datagrams are received or sent | `src/network/Transport/UdpSocket/` |
| Change how datagrams go between the socket and the queues | `src/network/Transport/DatagramRelay/` |
| Run a program over a simulated network | the options of [Network simulator](#network-simulator-simulatednetworksocket) |
| Change a scenario of the simulator, or add one | `NETWORK_SCENARIOS` and its entries in `src/network/Transport/Conditions/NetworkConditionsConstants.hpp` |
| Change what the simulator does to a datagram | `DatagramsInFlight` in `src/network/Transport/Simulator/` |
| Write a class that needs Asio | A folder in `src/network` whose target links `rtype_network_asio` privately |
| Change how an integer, a float or a string is encoded | `src/network/Serialization/ByteWriter/` and `ByteReader/`, then the [Protocol](/R-Type/protocol/#encoding) page |
| Change the string length limit or the sizes | `src/network/Serialization/SerializationConstants.hpp` |
| Change the layout or the limits of the world state | `src/network/Protocol/WorldStateCodec/`, the sizes in `src/network/Protocol/ProtocolConstants.hpp`, then the [Protocol](/R-Type/protocol/#world-state) page |
| Add a message, or change the fields or the checks of one | `src/network/Protocol/Messages/`, its value in `src/network/Protocol/MessageType.hpp`, then the [Protocol](/R-Type/protocol/#messages) page |
| Change the datagram header or how messages are framed | `src/network/Protocol/DatagramCodec/` and `PacketHeader.hpp`, then the [Protocol](/R-Type/protocol/#datagram) page |
| Change how a `bool`, a position or a nickname is checked | `src/network/Protocol/WireTypes.hpp`, and `Utf8Constants.hpp` for UTF-8 |
| Change how often a rejected datagram or message is logged | `REJECTION_LINE_INTERVAL` in `src/network/NetworkConstants.hpp`, read by the network and the server (see [Logging](/R-Type/logging/#repeated-events-throttledlog)) |
| Handle a received message | `registerHandler<Message>()` on a `MessageDispatcher` |
| Change how a connection cookie is computed, or how long it lives | `CookieIssuer` in `src/network/Handshake/CookieIssuer/`, `COOKIE_WINDOW_DURATION` in `src/network/Handshake/HandshakeConstants.hpp` |
| Add a network error | `src/network/Exceptions/NetworkException.hpp` |
| Test code against a real socket | `tests/UdpSocketTest.cpp`: a plain Asio client on the loopback address |
| Test code that uses a socket without the network | `tests/doubles/RecordingDatagramSocket.hpp`, as `tests/network/Transport/DatagramRelayTest.cpp` does |
| Build the bytes of a datagram in a test | `datagramOf()` and `defaultHeaderBytes()` in `tests/network/Protocol/ProtocolFixtures.hpp` |
| Check that a change survives malformed datagrams | `malformed_datagrams` (`tests/tools/`), see [Malformed datagrams](#malformed-datagrams-malformed_datagrams) |

Each folder has its own `CMakeLists.txt` declaring one library, linked into `rtype_network` by `src/network/CMakeLists.txt`.
