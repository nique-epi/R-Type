#include "ConfigurationReaderFixture.hpp"
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationReader.hpp"
#include "ConfigurationReaderTestConstants.hpp"
#include "ConfigurationRegistryTestConstants.hpp"
#include "ConfigurationReport.hpp"
#include "IntegerSettingDefinition.hpp"
#include "SettingValue.hpp"
#include "TextSettingDefinition.hpp"

ConfigurationReaderFixture::ConfigurationReaderFixture()
    : port_(&registry_.declareInteger(rtype::engine::IntegerSettingDefinition{
          .name = std::string(PORT_SETTING),
          .defaultValue = DEFAULT_PORT_VALUE,
          .lowest = LOWEST_PORT_VALUE,
          .highest = HIGHEST_PORT_VALUE,
          .help = std::string(PORT_SETTING)})),
      nickname_(&registry_.declareText(rtype::engine::TextSettingDefinition{
          .name = std::string(NICKNAME_SETTING),
          .defaultValue = std::string(DEFAULT_NICKNAME),
          .help = std::string(NICKNAME_SETTING),
          .validate = {}})) {}

void ConfigurationReaderFixture::writeFile(std::string_view content) const {
  folder_.write(std::string(CONFIGURATION_FILE), content);
}

std::filesystem::path ConfigurationReaderFixture::file() const {
  return folder_.path() / std::string(CONFIGURATION_FILE);
}

const TemporaryFolder& ConfigurationReaderFixture::folder() const {
  return folder_;
}

rtype::engine::CommandInterpreter& ConfigurationReaderFixture::interpreter() {
  return interpreter_;
}

const rtype::engine::SettingValue& ConfigurationReaderFixture::port() const {
  return *port_;
}

const rtype::engine::SettingValue& ConfigurationReaderFixture::nickname()
    const {
  return *nickname_;
}

rtype::engine::ConfigurationReport ConfigurationReaderFixture::read(
    const std::filesystem::path& path,
    const std::vector<std::string>& forcedLines) {
  return rtype::engine::readConfiguration(path, forcedLines, interpreter_);
}
