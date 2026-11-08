#include "WaveCatalogFixtures.hpp"
#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include "ServerConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogTestConstants.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The line `key = value`, ended. */
std::string line(std::string_view key, std::string_view value) {
  return std::string{key} + " = " + std::string{value} + '\n';
}

}  // namespace

std::string calmFile() { return calmFileWith({}, {}); }

std::string calmFileWith(std::string_view key, std::string_view value) {
  std::string text = std::string{CALM_HEADER} + '\n';
  bool replaced = false;
  for (const auto& [calmKey, calmValue] : CALM_VALUES) {
    const bool isTheKey = calmKey == key;
    replaced = replaced || isTheKey;
    text += line(calmKey, isTheKey ? value : calmValue);
  }
  if (!replaced && !key.empty()) {
    text += line(key, value);
  }
  return text;
}

std::string calmFileNamed(std::string_view name) {
  std::string text = calmFile();
  text.replace(0, CALM_HEADER.size(),
               std::string{DIFFICULTY_HEADER_START} + std::string{name} +
                   std::string{DIFFICULTY_HEADER_END});
  return text;
}

std::string fileOfDifficulties(std::size_t count) {
  std::string text;
  for (std::size_t index = 0; index < count; ++index) {
    text += calmFileNamed(std::string{NUMBERED_DIFFICULTY_PREFIX} +
                          std::to_string(index));
  }
  return text;
}

std::string calmFileWithout(std::string_view key) {
  std::string text = std::string{CALM_HEADER} + '\n';
  for (const auto& [calmKey, calmValue] : CALM_VALUES) {
    if (calmKey != key) {
      text += line(calmKey, calmValue);
    }
  }
  return text;
}

std::string errorReading(std::string_view text) {
  try {
    static_cast<void>(WaveCatalog::fromText(
        text, TEST_FILE_NAME, game::testing::shippedEntitySheets()));
  } catch (const std::runtime_error& error) {
    return error.what();
  }
  ADD_FAILURE() << "reading the file threw nothing";
  return {};
}

WaveCatalog shippedCatalog() {
  const std::array files{std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
                         WAVE_FILE_IN_ASSETS};
  return WaveCatalog::fromFiles(files, game::testing::shippedEntitySheets());
}

std::string withCrlfLineEndings(std::string_view text) {
  std::string converted;
  for (const char character : text) {
    if (character == '\n') {
      converted += '\r';
    }
    converted += character;
  }
  return converted;
}

}  // namespace rtype::server::testing
