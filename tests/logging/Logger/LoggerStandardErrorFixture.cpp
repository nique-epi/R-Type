#include "LoggerStandardErrorFixture.hpp"
#include <iostream>
#include <string>
#include "LogLevel.hpp"
#include "Logger.hpp"

using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::logging::LogOutput;

void LoggerStandardErrorFixture::SetUp() {
  savedLevel_ = Logger::level();
  Logger::setLevel(LogLevel::Info);
  Logger::setOutput(LogOutput{});
  originalBuffer_ = std::cerr.rdbuf(standardError_.rdbuf());
}

void LoggerStandardErrorFixture::TearDown() {
  std::cerr.rdbuf(originalBuffer_);
  Logger::setLevel(savedLevel_);
  Logger::setOutput(LogOutput{});
}

std::string LoggerStandardErrorFixture::standardErrorStream() const {
  return standardError_.str();
}
