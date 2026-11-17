#include "LoggerFixture.hpp"
#include "Logger.hpp"

void LoggerFixture::SetUp() {
  savedLevel_ = rtype::logging::Logger::level();
  rtype::logging::Logger::setOutput(rtype::logging::LogOutput{});
}

void LoggerFixture::TearDown() {
  rtype::logging::Logger::setLevel(savedLevel_);
  rtype::logging::Logger::setOutput(rtype::logging::LogOutput{});
}
