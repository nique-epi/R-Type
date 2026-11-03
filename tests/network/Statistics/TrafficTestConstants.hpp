#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief Bytes of a packet header and of a message prefix, from the protocol
 * page, and payloads of two sizes. */
constexpr std::size_t HEADER_BYTES = 22;
constexpr std::size_t PREFIX_BYTES = 3;
constexpr std::size_t LARGE_PAYLOAD = 100;
constexpr std::size_t SMALL_PAYLOAD = 10;
constexpr std::size_t EMPTY_PAYLOAD = 0;

/** @brief Type bytes no message has, for as long as there are fewer messages
 * than that. */
constexpr std::uint8_t TYPE_NO_MESSAGE_HAS = 200;
constexpr std::uint8_t LARGEST_TYPE_BYTE = 255;
constexpr std::uint8_t NO_MESSAGE_TYPE_ZERO = 0;

/** @brief A cut that leaves a datagram too short for a header. */
constexpr std::size_t FIVE_BYTES = 5;

/** @brief A datagram cut at each of these sizes holds no whole message: empty,
 * one byte, a byte short of a header, and a header alone. */
constexpr std::array<std::size_t, 4> TOO_SHORT_SIZES{0, 1, 21, 22};

/** @brief The ports of two peers on the loopback address. */
constexpr std::uint16_t PEER_PORT = 5000;
constexpr std::uint16_t OTHER_PORT = 5001;

/** @brief Senders of a flood, each from a port of its own. */
constexpr std::uint16_t FLOOD_SENDERS = 1000;

/** @brief The work of a writer and a reader on two threads. */
constexpr std::size_t DATAGRAMS_WHILE_READING = 10000;
constexpr std::size_t REPORTS_WHILE_COUNTING = 1000;

/** @brief Times on the clock of a recorder. */
constexpr std::chrono::seconds RECORDER_START{5};
constexpr std::chrono::milliseconds HALF_A_SECOND{500};
constexpr std::chrono::seconds TEN_SECONDS{10};
constexpr std::chrono::milliseconds ONE_AND_A_HALF_SECONDS{1500};

/** @brief The first line of a traffic file, and what marks a line of the
 * journal or a warning about the file. */
constexpr std::string_view HEADER_LINE =
    "seconds,peer,direction,message,count,bytes\n";
constexpr std::string_view TRAFFIC_LINE_MARK = "[Traffic]";
constexpr std::string_view UNWRITABLE_FILE_MARK = "cannot be written";

/** @brief How a row of a traffic file ends, for a datagram of one byte sent to
 * someone nobody follows. */
constexpr std::string_view ONE_BYTE_SENT_ROW_END = ",others,sent,all,1,1\n";

/** @brief A device that refuses every write, where the system has one. */
constexpr std::string_view FULL_DISK_DEVICE = "/dev/full";
