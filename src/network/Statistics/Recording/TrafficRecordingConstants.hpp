#pragma once

#include <chrono>
#include <string_view>

namespace rtype::network {

/** @brief Module name the traffic of a program writes its log lines under. */
constexpr std::string_view TRAFFIC_LOGGER_NAME = "Traffic";

/** @brief How often the traffic goes to its file, and to the journal. */
constexpr std::chrono::seconds TRAFFIC_CSV_PERIOD{1};
constexpr std::chrono::seconds TRAFFIC_LINE_PERIOD{10};

/** @brief The first line of a traffic file, and the digits after the point of
 * its first column. */
constexpr std::string_view TRAFFIC_CSV_HEADER =
    "seconds,peer,direction,message,count,bytes";
constexpr int TRAFFIC_CSV_SECONDS_DECIMALS = 3;

/** @brief The texts of the columns of a traffic file that are not a peer's
 * address or the name of a message. */
constexpr std::string_view OTHERS_PEER_TEXT = "others";
constexpr std::string_view DEPARTED_PEER_TEXT = "departed";
constexpr std::string_view SENT_DIRECTION_TEXT = "sent";
constexpr std::string_view RECEIVED_DIRECTION_TEXT = "received";
constexpr std::string_view ALL_MESSAGES_TEXT = "all";
constexpr std::string_view UNREADABLE_MESSAGES_TEXT = "unreadable";

/** @brief What a traffic file calls the messages of a type byte no message
 * has. */
constexpr std::string_view UNKNOWN_MESSAGE_TEXT = "unknown";

}  // namespace rtype::network
