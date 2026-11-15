#include "LogLaunchOptionsFixture.hpp"
#include <iostream>
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
  originalBuffer_ = std::cerr.rdbuf(standardError_.rdbuf());
}

void LogLaunchOptionsFixture::TearDown() {
  rtype::logging::flushJournal();
  std::cerr.rdbuf(originalBuffer_);
  Logger::setLevel(savedLevel_);
  Logger::setOutput(LogOutput{});
}

std::string LogLaunchOptionsFixture::standardError() const {
  rtype::logging::flushJournal();
  return standardError_.str();
}
