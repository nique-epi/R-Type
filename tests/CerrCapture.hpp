#pragma once

#include <sstream>
#include <streambuf>
#include <string>

/** @brief Takes what reaches std::cerr for its life, then gives std::cerr its
 * buffer back; both read and give back once the journal wrote what was
 * queued. */
class CerrCapture {
 public:
  CerrCapture();
  ~CerrCapture();

  CerrCapture(const CerrCapture&) = delete;
  CerrCapture& operator=(const CerrCapture&) = delete;
  CerrCapture(CerrCapture&&) = delete;
  CerrCapture& operator=(CerrCapture&&) = delete;

  /** @returns What reached std::cerr so far. */
  [[nodiscard]] std::string str() const;

 private:
  std::streambuf* oldBuffer_;
  std::stringstream buffer_;
};
