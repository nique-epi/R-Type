#include "MasterAccountClientFixtures.hpp"
#include <string>
#include "ExchangeReceipt.hpp"
#include "MasterAccountClientTestConstants.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "UserRole.hpp"
#include "WalletExchange.hpp"

rtype::network::User aceUser() {
  return rtype::network::User{.id = ACE_USER_ID,
                              .nickname = std::string{ACE_USER_NICKNAME},
                              .role = rtype::network::UserRole::Player};
}

rtype::network::OwnedLooks aceLooks() {
  return rtype::network::OwnedLooks{
      .items = {"top_hat", "crown", "cool_shades", "mustache"}};
}

rtype::network::ExchangeReceipt buyFiveHundredChipsReceipt() {
  return rtype::network::ExchangeReceipt{
      .exchange =
          rtype::network::WalletExchange{.id = BUY_500_CHIPS_ID,
                                         .from = BUY_500_CHIPS.from,
                                         .amount = BUY_500_CHIPS.amount,
                                         .madeAt = BUY_500_CHIPS_MADE_AT},
      .wallet = WALLET_AFTER};
}
