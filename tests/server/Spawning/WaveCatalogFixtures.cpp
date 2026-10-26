#include "WaveCatalogFixtures.hpp"
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "ServerException.hpp"
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
    static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME));
  } catch (const ServerException& error) {
    return error.what();
  }
  ADD_FAILURE() << "reading the file threw nothing";
  return {};
}

}  // namespace rtype::server::testing
