#include "LogLaunchOptionsFixture.hpp"
#include <string>
#include "LogLevel.hpp"
#include "Logger.hpp"

using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::logging::LogOutput;

void LogLaunchOptionsFixture::SetUp() {
  savedLevel_ = Logger::level();
  Logger::setLevel(LogLevel::Warn);
  Logger::setOutput(LogOutput{});
}

void LogLaunchOptionsFixture::TearDown() {
  Logger::setLevel(savedLevel_);
  Logger::setOutput(LogOutput{});
}

std::string LogLaunchOptionsFixture::standardError() const {
  return standardError_.str();
}
