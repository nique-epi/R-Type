#include "WaveCatalogLayersFixtures.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>
#include "ShippedEntitySheets.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogLayersTestConstants.hpp"
#include "WaveFileText.hpp"

namespace rtype::server::testing {

WaveCatalog laid(std::span<const std::string> texts) {
  std::vector<WaveFileText> files;
  files.reserve(texts.size());
  for (std::size_t index = 0; index < texts.size(); ++index) {
    files.push_back(
        WaveFileText{.text = texts[index],
                     .source = std::string{LAYERED_FILE_NAMES.at(index)}});
  }
  return WaveCatalog::fromTexts(files, game::testing::shippedEntitySheets());
}

std::string errorLaying(std::span<const std::string> texts) {
  try {
    static_cast<void>(laid(texts));
  } catch (const std::runtime_error& error) {
    return error.what();
  }
  ADD_FAILURE() << "laying the files threw nothing";
  return {};
}

}  // namespace rtype::server::testing
