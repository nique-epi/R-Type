#include "CsvFile.hpp"
#include <filesystem>
#include <ostream>
#include <string_view>
#include "NetworkException.hpp"
#include "PathText.hpp"

namespace rtype::network {

CsvFile::CsvFile(const std::filesystem::path& path, std::string_view header)
    : file_(path) {
  if (!file_.is_open()) {
    throw CsvFileOpenException(logging::genericText(path));
  }
  file_ << header << '\n' << std::flush;
}

bool CsvFile::append(std::string_view rows) {
  file_ << rows << std::flush;
  return file_.good();
}

}  // namespace rtype::network
