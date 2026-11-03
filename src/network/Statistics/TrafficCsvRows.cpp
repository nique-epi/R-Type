#include "TrafficCsvRows.hpp"
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include "DirectionTraffic.hpp"
#include "Endpoint.hpp"
#include "MessageTypeText.hpp"
#include "PeerTraffic.hpp"
#include "StatisticsConstants.hpp"
#include "TrafficFigures.hpp"
#include "TrafficReport.hpp"
#include "TwoWayTraffic.hpp"

namespace rtype::network {

namespace {

std::string secondsText(std::chrono::nanoseconds elapsed) {
  std::ostringstream text;
  text << std::fixed << std::setprecision(TRAFFIC_CSV_SECONDS_DECIMALS)
       << std::chrono::duration<double>(elapsed).count();
  return text.str();
}

std::string peerText(const Endpoint& peer) {
  return peer.address + ':' + std::to_string(peer.port);
}

/** @returns The last two columns of a row, each after a comma, and the end of
 * the line. */
std::string figuresText(const TrafficFigures& figures) {
  return ',' + std::to_string(figures.count) + ',' +
         std::to_string(figures.bytes) + '\n';
}

/** @param start The columns before the message, each followed by a comma. */
std::string directionRows(const std::string& start,
                          const DirectionTraffic& traffic) {
  if (traffic.datagrams.count == 0) {
    return {};
  }
  std::string rows =
      start + std::string{ALL_MESSAGES_TEXT} + figuresText(traffic.datagrams);
  if (traffic.unreadable.count > 0) {
    rows += start + std::string{UNREADABLE_MESSAGES_TEXT} +
            figuresText(traffic.unreadable);
  }
  for (std::size_t type = 0; type < traffic.messages.size(); ++type) {
    if (traffic.messages[type].count > 0) {
      rows +=
          start + messageTypeText(type) + figuresText(traffic.messages[type]);
    }
  }
  return rows;
}

std::string twoWayRows(const std::string& start, const TwoWayTraffic& traffic) {
  return directionRows(start + std::string{SENT_DIRECTION_TEXT} + ',',
                       traffic.sent) +
         directionRows(start + std::string{RECEIVED_DIRECTION_TEXT} + ',',
                       traffic.received);
}

}  // namespace

std::string trafficCsvRowsOf(std::chrono::nanoseconds elapsed,
                             const TrafficReport& report) {
  const std::string seconds = secondsText(elapsed) + ',';
  std::string rows;
  for (const PeerTraffic& peer : report.peers) {
    rows += twoWayRows(seconds + peerText(peer.peer) + ',', peer.traffic);
  }
  rows +=
      twoWayRows(seconds + std::string{OTHERS_PEER_TEXT} + ',', report.others);
  rows += twoWayRows(seconds + std::string{DEPARTED_PEER_TEXT} + ',',
                     report.departed);
  return rows;
}

}  // namespace rtype::network
