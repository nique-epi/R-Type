#include "ConfigurationLogFixture.hpp"
#include <string>
#include <vector>
#include "ConfigurationLogTestConstants.hpp"
#include "Logger.hpp"

ConfigurationLogFixture::ConfigurationLogFixture()
    : logger_(std::string{LOGGED_MODULE_NAME}),
      previousLevel_(rtype::logging::Logger::level()) {
  rtype::logging::Logger::setLevel(rtype::logging::LogLevel::Info);
  rtype::logging::Logger::setOutput(rtype::logging::LogOutput{
      .toStderr = false, .filePath = journal_.path()});
}

ConfigurationLogFixture::~ConfigurationLogFixture() {
  rtype::logging::Logger::setLevel(previousLevel_);
}

std::vector<std::string> ConfigurationLogFixture::lines() const {
  return journal_.lines();
}

const rtype::logging::Logger& ConfigurationLogFixture::logger() const {
  return logger_;
}
