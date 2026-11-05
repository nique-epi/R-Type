#include "TrafficCsvFile.hpp"
#include <filesystem>
#include <ostream>
#include <string_view>
#include "NetworkException.hpp"
#include "PathText.hpp"
#include "TrafficRecordingConstants.hpp"

namespace rtype::network {

TrafficCsvFile::TrafficCsvFile(const std::filesystem::path& path)
    : file_(path) {
  if (!file_.is_open()) {
    throw TrafficFileOpenException(logging::genericText(path));
  }
  file_ << TRAFFIC_CSV_HEADER << '\n' << std::flush;
}

bool TrafficCsvFile::append(std::string_view rows) {
  file_ << rows << std::flush;
  return file_.good();
}

}  // namespace rtype::network
