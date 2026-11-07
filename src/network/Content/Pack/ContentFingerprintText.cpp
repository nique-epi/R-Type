#include "ContentFingerprintText.hpp"
#include <string>
#include "ContentFingerprint.hpp"
#include "Hashing.hpp"

namespace rtype::network {

std::string fingerprintText(const ContentFingerprint& fingerprint) {
  return hexadecimalOf(fingerprint.bytes);
}

}  // namespace rtype::network
