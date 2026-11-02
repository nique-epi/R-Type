#include "LogLaunchOptionsFixture.hpp"
#include <iostream>
#include <string>
#include "Logger.hpp"

using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::logging::LogOutput;

void LogLaunchOptionsFixture::SetUp() {
  savedLevel_ = Logger::level();
  Logger::setLevel(LogLevel::Warn);
  Logger::setOutput(LogOutput{});
  originalBuffer_ = std::cerr.rdbuf(standardError_.rdbuf());
}

void LogLaunchOptionsFixture::TearDown() {
  std::cerr.rdbuf(originalBuffer_);
  Logger::setLevel(savedLevel_);
  Logger::setOutput(LogOutput{});
}

std::string LogLaunchOptionsFixture::standardError() const {
  return standardError_.str();
}
