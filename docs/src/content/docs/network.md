---
title: Network
description: What the network library does, how its parts fit together and where to find them in the code.
---

The network library, `rtype_network`, carries datagrams between the programs. Today it opens UDP sockets, sends datagrams and hands over the ones it receives, with their sender. It knows nothing about entities or the rules of the game, and it is the only library that includes Asio: the client and the server use it through classes that never name an Asio type (see [Project layout](/R-Type/project-layout/#network)). What the datagrams contain is described in [Protocol](/R-Type/protocol/).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Event loop | Available | `src/network/NetworkContext/` |
| UDP socket | Available | `src/network/Transport/` |
| Lag and loss simulator | Not yet written | — |
| Serialization, protocol messages | Not yet written | — |
| Handshake, sessions, reliability, snapshots, statistics | Not yet written | — |

## Event loop: `NetworkContext`

`NetworkContext` owns the Asio event loop. `run()` blocks and runs every network operation, and every socket handler, on the thread that called it. It returns when the loop has no work left or `stop()` is called; a socket that is receiving always has work, so a server returns only on `stop()`.

Asio is linked privately to the network targets, so the headers other modules include never include it, and `r-type_server` fails to build if it tries. A class that needs Asio lives in `src/network` and links `rtype_network_asio`, which also defines `ASIO_NO_DEPRECATED` and, on Windows, `_WIN32_WINNT`.

## Sockets: `UdpSocket`

`UdpSocket` (`src/network/Transport/UdpSocket/`) implements `IDatagramSocket`. It opens a UDP socket on an `Endpoint`, sends datagrams and hands every datagram received to a handler. None of its calls blocks. Code that only sends and receives can take an `IDatagramSocket&`, so a lag and loss simulator or a test double can stand in for the real socket.

- An `Endpoint` is an IP address written as text, `"127.0.0.1"` or `"::1"`, and a port. Host names are never resolved: `"localhost"` throws `InvalidAddressException`. Port 0 lets the system pick a free port, which `localEndpoint()` returns. A port already taken throws `SocketOpenException`.
- `startReceiving(handler)` is called once; a second call throws `ReceivingAlreadyStartedException`. The handler gets the sender and a `std::span` of the bytes, valid only during the call. It must not throw: an exception leaves `run()` and the socket stops receiving.
- A datagram larger than `MAX_DATAGRAM_SIZE` (1200 bytes) never reaches the handler. The socket receives into `RECEIVE_BUFFER_SIZE` (1201) bytes, so a datagram cut down to the buffer by the system is told apart from one of exactly 1200 bytes. An empty datagram arrives as an empty span: checking sizes is the job of the code that decodes it.
- `send(destination, payload)` copies the payload and returns at once. More than `MAX_DATAGRAM_SIZE` bytes throws `DatagramTooLargeException` and sends nothing. A failed send is logged, and UDP never tells whether a datagram arrived.
- An error while receiving is logged and the socket keeps receiving. On Windows, a datagram sent to a port nobody listens on makes a later receive fail with "connection refused"; it is logged at the `debug` level.
- A socket is not thread-safe: use it and destroy it on the thread that runs its `NetworkContext`. Once it is destroyed, its handler is never called again, even for a datagram already received.

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

`r-type_server` opens such a socket on `DEFAULT_SERVER_PORT` (4242), on every IPv4 interface, and only logs each datagram at the `debug` level: nothing decodes or answers them yet.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change the largest datagram | `MAX_DATAGRAM_SIZE` in `src/network/NetworkConstants.hpp`, and the Transport section of [Protocol](/R-Type/protocol/#transport) |
| Change the port the server listens on by default | `DEFAULT_SERVER_PORT` in `src/network/NetworkConstants.hpp` |
| Change how datagrams are received or sent | `src/network/Transport/UdpSocket/` |
| Write a class that needs Asio | A folder in `src/network` whose target links `rtype_network_asio` privately |
| Add a network error | `src/network/Exceptions/NetworkException.hpp` |
| Test code against a real socket | `tests/UdpSocketTest.cpp`: a plain Asio client on the loopback address |

Each folder has its own `CMakeLists.txt` declaring one library, linked into `rtype_network` by `src/network/CMakeLists.txt`.
