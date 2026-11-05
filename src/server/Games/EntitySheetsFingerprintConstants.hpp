#pragma once

#include "SipHash.hpp"

namespace rtype::server {

/**
 * @brief The key entitySheetsFingerprint() hashes the entity sheets under.
 * Anyone may know it: the fingerprint tells two sets of sheets apart, it keeps
 * no secret.
 */
constexpr network::SipHashKey ENTITY_SHEETS_FINGERPRINT_KEY{};

/** @brief What ends the name and the text of each sheet in the bytes hashed,
 * so that no text can be read as the end of a name. */
constexpr char SHEET_TEXT_END = '\0';

}  // namespace rtype::server
