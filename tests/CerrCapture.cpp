#include "CerrCapture.hpp"
#include <iostream>
#include <string>
#include "Logger.hpp"

CerrCapture::CerrCapture() : oldBuffer_(std::cerr.rdbuf()) {
  std::cerr.rdbuf(buffer_.rdbuf());
}

CerrCapture::~CerrCapture() {
  rtype::logging::flushJournal();
  std::cerr.rdbuf(oldBuffer_);
}

std::string CerrCapture::str() const {
  rtype::logging::flushJournal();
  return buffer_.str();
}
