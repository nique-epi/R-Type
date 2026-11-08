#include "BanList.hpp"
#include <chrono>
#include <utility>
#include <vector>
#include "Ban.hpp"
#include "BanCandidate.hpp"
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "WireTypes.hpp"

namespace rtype::server {

namespace {

/** @returns Whether @p first and @p second name one target. */
bool sameTarget(const BanTarget& first, const BanTarget& second) {
  if (first.kind != second.kind) {
    return false;
  }
  return first.kind == BanKind::Nickname ? network::nicknameKey(first.text) ==
                                               network::nicknameKey(second.text)
                                         : first.text == second.text;
}

/** @returns Whether @p ban names @p candidate. */
bool names(const Ban& ban, const BanCandidate& candidate) {
  return ban.target.kind == BanKind::Nickname
             ? network::nicknameKey(ban.target.text) ==
                   network::nicknameKey(candidate.nickname)
             : ban.target.text == candidate.address;
}

/** @returns Whether @p ban has not ended at @p at. */
bool inForce(const Ban& ban, std::chrono::sys_seconds at) {
  return !ban.expiresAt.has_value() || *ban.expiresAt > at;
}

}  // namespace

BanList::BanList(std::vector<Ban> bans) : bans_(std::move(bans)) {}

const Ban* BanList::barring(const BanCandidate& candidate) const {
  for (const Ban& ban : bans_) {
    if (inForce(ban, candidate.at) && names(ban, candidate)) {
      return &ban;
    }
  }
  return nullptr;
}

void BanList::put(Ban ban) {
  static_cast<void>(remove(ban.target));
  bans_.push_back(std::move(ban));
}

bool BanList::remove(const BanTarget& target) {
  return std::erase_if(bans_, [&target](const Ban& ban) {
           return sameTarget(ban.target, target);
         }) > 0;
}

const std::vector<Ban>& BanList::bans() const { return bans_; }

}  // namespace rtype::server
