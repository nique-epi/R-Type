#include "TrafficLine.hpp"
#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include "TrafficFigures.hpp"
#include "TrafficTotals.hpp"

namespace rtype::network {

namespace {

std::uint64_t bytesPerSecond(std::uint64_t bytes, double seconds) {
  if (seconds <= 0.0) {
    return 0;
  }
  return static_cast<std::uint64_t>(static_cast<double>(bytes) / seconds);
}

std::string directionText(const TrafficFigures& older,
                          const TrafficFigures& newer, double seconds) {
  const std::uint64_t bytes = newer.bytes - older.bytes;
  std::ostringstream text;
  text << newer.count - older.count << " datagrams, " << bytes << " bytes ("
       << bytesPerSecond(bytes, seconds) << " B/s)";
  return text.str();
}

}  // namespace

std::string trafficLineOf(const TrafficTotals& older,
                          const TrafficTotals& newer,
                          std::chrono::nanoseconds elapsed) {
  const double seconds = std::chrono::duration<double>(elapsed).count();
  std::ostringstream line;
  line << "traffic over " << seconds << " s: sent "
       << directionText(older.sent, newer.sent, seconds) << "; received "
       << directionText(older.received, newer.received, seconds);
  return line.str();
}

}  // namespace rtype::network
