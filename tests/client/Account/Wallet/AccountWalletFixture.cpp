#include "AccountWalletFixture.hpp"
#include <string>
#include "AccountTestConstants.hpp"
#include "AccountWallet.hpp"
#include "FakeAccountSession.hpp"
#include "RecordingAccountLink.hpp"
#include "WalletReading.hpp"

namespace rtype::client::testing {

AccountWalletFixture::AccountWalletFixture() : wallet_(session_, link_) {}

RecordingAccountLink& AccountWalletFixture::link() { return link_; }

FakeAccountSession& AccountWalletFixture::session() { return session_; }

AccountWallet& AccountWalletFixture::wallet() { return wallet_; }

void AccountWalletFixture::readAceWalletWith(const std::string& token) {
  session_.signInAs(ACE_ACCOUNT_ID, std::string{ACE_TOKEN});
  link_.walletRead = WalletReading{.ask = FIRST_ASK,
                                   .token = token,
                                   .wallet = ACE_BALANCES,
                                   .looks = CROWN_ONLY};
}

}  // namespace rtype::client::testing
