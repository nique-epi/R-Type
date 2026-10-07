---
title: Network
description: What the network library does, how its parts fit together and where to find them in the code.
---

The network library (`rtype_network`) is everything that crosses the wire: how values become bytes, and later how datagrams travel and are checked. It knows nothing about entities, SFML or the rules of the game (see [Architecture](/R-Type/architecture/)). The exact bytes of each message are on the [Protocol](/R-Type/protocol/) page.

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Event loop | Available | `src/network/NetworkContext.hpp` |
| Errors | Available | `src/network/Exceptions/` |
| Serialization | Available | `src/network/Serialization/` |
| Transport, protocol messages, handshake, sessions | Not yet written | — |

## Serialization: `ByteWriter` and `ByteReader`

`ByteWriter` appends values to a `std::vector<std::byte>` and `ByteReader` reads them back from a buffer. Every value is written field by field, never copied from a struct in memory, so neither padding nor the byte order of the machine reaches the wire.

| Call | On the wire |
|---|---|
| `writeUint8` to `writeUint64`, `writeInt8` to `writeInt64` | 1, 2, 4 or 8 bytes, big-endian; signed values in two's complement |
| `writeFloat` | 4 bytes, IEEE-754 binary32, big-endian |
| `writeString` | one length byte, then the characters, at most `MAX_SHORT_STRING_LENGTH` (255) |

The build fails on a platform where `float` is not binary32. Version 0 of the protocol sends no float (positions are quantized integers), so `writeFloat` is there for later use.

`ByteReader` does not own its buffer: the caller keeps the bytes alive while it reads. Every read first compares what it needs with `remaining()`. A read that needs more throws `BufferUnderflowException`, and the position does not move, so a truncated or forged datagram is rejected without reading outside the buffer. A string announcing more characters than follow is rejected the same way, before anything is allocated.

`writeString` throws `StringTooLongException` for more than 255 bytes and appends nothing. It copies the bytes as they are and the reader does not check that they are UTF-8, so a caller that needs valid text must check it.

Both exceptions derive from `NetworkException`, the root of every error of this library, so one `catch` handles them all.

```cpp
rtype::network::ByteWriter writer;
writer.writeUint16(sequence);
writer.writeString(playerName);

rtype::network::ByteReader reader(writer.bytes());
const auto decodedSequence = reader.readUint16();
const auto decodedName = reader.readString();
```

The `bool`, `position` and enumeration types of the protocol are not provided yet: the message code will build them on top of these calls.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change how an integer, a float or a string is encoded | `src/network/Serialization/ByteWriter/` and `ByteReader/`, then the [Protocol](/R-Type/protocol/#encoding) page |
| Change the string length limit or the sizes | `src/network/Serialization/SerializationConstants.hpp` |
| Add an error of this library | `src/network/Exceptions/` |
