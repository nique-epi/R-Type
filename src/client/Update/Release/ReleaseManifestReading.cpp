#include "ReleaseManifestReading.hpp"
#include <algorithm>
#include <cstddef>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ReleaseConstants.hpp"
#include "ReleaseOffer.hpp"
#include "ReleasePackage.hpp"
#include "ReleaseVersion.hpp"

namespace rtype::client {

namespace {

bool isDigit(char character) { return character >= '0' && character <= '9'; }

bool isFileNameCharacter(char character) {
  return (character >= 'a' && character <= 'z') ||
         (character >= 'A' && character <= 'Z') || isDigit(character) ||
         character == '.' || character == '-' || character == '_';
}

bool isPackageFileName(std::string_view name) {
  return !name.empty() && name.size() <= MAXIMUM_PACKAGE_FILE_NAME_SIZE &&
         name.front() != '.' && std::ranges::all_of(name, isFileNameCharacter);
}

bool isSha256Text(std::string_view digest) {
  return digest.size() == SHA256_HEXADECIMAL_SIZE &&
         std::ranges::all_of(digest, [](char character) {
           return isDigit(character) || (character >= 'a' && character <= 'f');
         });
}

bool isNoteLine(std::string_view line) {
  return line.size() <= MAXIMUM_NOTE_SIZE &&
         std::ranges::all_of(line, [](char character) {
           return character >= FIRST_PRINTABLE_CHARACTER &&
                  character <= LAST_PRINTABLE_CHARACTER;
         });
}

/** @returns The text of member @p key of @p object, when it is a string. */
std::optional<std::string> textAt(const nlohmann::json& object,
                                  std::string_view key) {
  const auto member = object.find(key);
  if (member == object.end() || !member->is_string()) {
    return std::nullopt;
  }
  return member->get<std::string>();
}

std::optional<std::vector<std::string>> notesOf(const nlohmann::json& root) {
  const auto member = root.find(MANIFEST_NOTES_KEY);
  if (member == root.end() || !member->is_array() ||
      member->size() > MAXIMUM_NOTE_LINES) {
    return std::nullopt;
  }
  std::vector<std::string> notes;
  for (const nlohmann::json& line : *member) {
    if (!line.is_string() || !isNoteLine(line.get<std::string>())) {
      return std::nullopt;
    }
    notes.push_back(line.get<std::string>());
  }
  return notes;
}

/** @returns The size of @p package, when it is a whole number from 1 to
 * MAXIMUM_PACKAGE_BYTES. */
std::optional<std::size_t> sizeOf(const nlohmann::json& package) {
  const auto size = package.find(PACKAGE_SIZE_KEY);
  if (size == package.end() || !size->is_number_unsigned()) {
    return std::nullopt;
  }
  const auto bytes = size->get<std::size_t>();
  if (bytes == 0 || bytes > MAXIMUM_PACKAGE_BYTES) {
    return std::nullopt;
  }
  return bytes;
}

std::optional<ReleasePackage> packageOf(const nlohmann::json& root,
                                        std::string_view system) {
  const auto packages = root.find(MANIFEST_PACKAGES_KEY);
  if (packages == root.end() || !packages->is_object()) {
    return std::nullopt;
  }
  const auto package = packages->find(system);
  if (package == packages->end() || !package->is_object()) {
    return std::nullopt;
  }
  std::optional<std::string> file = textAt(*package, PACKAGE_FILE_KEY);
  std::optional<std::string> sha256 = textAt(*package, PACKAGE_SHA256_KEY);
  const std::optional<std::size_t> size = sizeOf(*package);
  if (!file.has_value() || !isPackageFileName(*file) || !sha256.has_value() ||
      !isSha256Text(*sha256) || !size.has_value()) {
    return std::nullopt;
  }
  return ReleasePackage{
      .file = std::move(*file), .size = *size, .sha256 = std::move(*sha256)};
}

}  // namespace

std::optional<ReleaseOffer> releaseOfferOf(std::string_view manifest,
                                           std::string_view system) {
  const nlohmann::json root = nlohmann::json::parse(manifest, nullptr, false);
  if (root.is_discarded() || !root.is_object()) {
    return std::nullopt;
  }
  std::optional<std::string> versionText = textAt(root, MANIFEST_VERSION_KEY);
  if (!versionText.has_value()) {
    return std::nullopt;
  }
  const std::optional<ReleaseVersion> version = releaseVersionOf(*versionText);
  std::optional<std::vector<std::string>> notes = notesOf(root);
  std::optional<ReleasePackage> package = packageOf(root, system);
  if (!version.has_value() || !notes.has_value() || !package.has_value()) {
    return std::nullopt;
  }
  return ReleaseOffer{.version = *version,
                      .versionText = std::move(*versionText),
                      .notes = std::move(*notes),
                      .package = std::move(*package)};
}

}  // namespace rtype::client
