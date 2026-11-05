#include "EntitySheetsFingerprint.hpp"
#include <cstdint>
#include <span>
#include <string>
#include "EntitySheets.hpp"
#include "EntitySheetsFingerprintConstants.hpp"
#include "SheetText.hpp"
#include "SipHash.hpp"

namespace rtype::server {

std::uint64_t entitySheetsFingerprint(const game::EntitySheets& types) {
  std::string hashed;
  for (const game::SheetText& sheet : types.texts()) {
    hashed += sheet.name;
    hashed += SHEET_TEXT_END;
    hashed += sheet.text;
    hashed += SHEET_TEXT_END;
  }
  return network::sipHash(ENTITY_SHEETS_FINGERPRINT_KEY,
                          std::as_bytes(std::span{hashed}));
}

}  // namespace rtype::server
