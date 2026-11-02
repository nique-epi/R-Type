#include "ConfigurationReaderFixture.hpp"
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "ConfigurationReader.hpp"
#include "ConfigurationReaderTestConstants.hpp"
#include "ConfigurationRegistryTestConstants.hpp"
#include "IntegerSettingDefinition.hpp"
#include "TextSettingDefinition.hpp"

ConfigurationReaderFixture::ConfigurationReaderFixture()
    : port_(&registry_.declareInteger(rtype::engine::IntegerSettingDefinition{
          .name = std::string(PORT_SETTING),
          .defaultValue = DEFAULT_PORT_VALUE,
          .lowest = LOWEST_PORT_VALUE,
          .highest = HIGHEST_PORT_VALUE})),
      nickname_(&registry_.declareText(rtype::engine::TextSettingDefinition{
          .name = std::string(NICKNAME_SETTING),
          .defaultValue = std::string(DEFAULT_NICKNAME)})) {}

void ConfigurationReaderFixture::writeFile(std::string_view content) const {
  folder_.write(std::string(CONFIGURATION_FILE), content);
}

std::filesystem::path ConfigurationReaderFixture::file() const {
  return folder_.path() / std::string(CONFIGURATION_FILE);
}

rtype::engine::ConfigurationReport ConfigurationReaderFixture::read(
    const std::filesystem::path& path,
    const std::vector<std::string>& forcedLines) const {
  return rtype::engine::readConfiguration(path, forcedLines, interpreter_);
}
