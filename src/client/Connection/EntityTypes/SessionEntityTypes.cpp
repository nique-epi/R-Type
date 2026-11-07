#include "SessionEntityTypes.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "ContentFingerprint.hpp"
#include "EntityAppearances.hpp"
#include "EntityTypeConstants.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityType.hpp"
#include "PackContent.hpp"

namespace rtype::client {

SessionEntityTypes::SessionEntityTypes(LocalContent local)
    : none_(EntityAppearances::fromTexts({})), local_(std::move(local)) {}

void SessionEntityTypes::drawLocalWith(const EntityAppearances& appearances) {
  localAppearances_ = &appearances;
}

const network::ContentFingerprint& SessionEntityTypes::localFingerprint()
    const {
  return local_.fingerprint;
}

void SessionEntityTypes::use(PackContent pack) {
  pack_ = std::move(pack);
  playsThePack_ = true;
}

void SessionEntityTypes::forget() { playsThePack_ = false; }

const game::NumberedEntityType* SessionEntityTypes::entryNumbered(
    std::uint8_t number) const {
  const std::vector<game::NumberedEntityType>& numbered = types();
  if (number == identifiers::NO_ENTITY_TYPE ||
      static_cast<std::size_t>(number) > numbered.size()) {
    return nullptr;
  }
  return &numbered[static_cast<std::size_t>(number) - 1];
}

const EntityAppearances& SessionEntityTypes::appearances() const {
  if (playsThePack_ && pack_.has_value()) {
    return pack_->appearances;
  }
  return localAppearances_ != nullptr ? *localAppearances_ : none_;
}

const std::vector<game::NumberedEntityType>& SessionEntityTypes::types() const {
  return playsThePack_ && pack_.has_value() ? pack_->types : local_.types;
}

}  // namespace rtype::client
