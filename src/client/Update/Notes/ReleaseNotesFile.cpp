#include "ReleaseNotesFile.hpp"
#include <filesystem>
#include <fstream>
#include <ios>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <system_error>
#include "ReleaseNotes.hpp"
#include "UpdateConstants.hpp"

namespace rtype::client {

bool writeReleaseNotes(const std::filesystem::path& file,
                       const ReleaseNotes& notes) {
  std::error_code error;
  std::filesystem::create_directories(file.parent_path(), error);
  nlohmann::json content = nlohmann::json::object();
  content[std::string{NOTES_VERSION_KEY}] = notes.versionText;
  content[std::string{NOTES_LINES_KEY}] = notes.lines;
  std::ofstream output{file, std::ios::trunc};
  output << content.dump();
  return static_cast<bool>(output.flush());
}

std::optional<ReleaseNotes> readReleaseNotes(
    const std::filesystem::path& file) {
  std::ifstream input{file};
  if (!input) {
    return std::nullopt;
  }
  const nlohmann::json content = nlohmann::json::parse(input, nullptr, false);
  if (content.is_discarded() || !content.is_object()) {
    return std::nullopt;
  }
  const auto version = content.find(NOTES_VERSION_KEY);
  const auto lines = content.find(NOTES_LINES_KEY);
  if (version == content.end() || !version->is_string() ||
      lines == content.end() || !lines->is_array()) {
    return std::nullopt;
  }
  ReleaseNotes notes{.versionText = version->get<std::string>(), .lines = {}};
  for (const nlohmann::json& line : *lines) {
    if (!line.is_string()) {
      return std::nullopt;
    }
    notes.lines.push_back(line.get<std::string>());
  }
  return notes;
}

}  // namespace rtype::client
