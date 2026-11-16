#include "RecordingAccountLink.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "ExchangeRequest.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

std::uint64_t RecordingAccountLink::askUser(const std::string& token) {
  asked.userToken = token;
  return ++asked.userAsk;
}

void RecordingAccountLink::askWallet(const std::string& token) {
  asked.token = token;
  ++asked.walletAsk;
}

std::uint64_t RecordingAccountLink::askPurchase(const std::string& token,
                                                std::string_view item) {
  asked.token = token;
  asked.purchaseItem = std::string{item};
  return ++asked.purchaseAsk;
}

std::uint64_t RecordingAccountLink::askExchange(
    const std::string& token, const rtype::network::ExchangeRequest& request) {
  asked.token = token;
  asked.exchangeRequest = request;
  return ++asked.exchangeAsk;
}

std::optional<rtype::client::UserReading> RecordingAccountLink::user() const {
  return userRead;
}

std::optional<rtype::client::WalletReading> RecordingAccountLink::wallet()
    const {
  return walletRead;
}

std::optional<rtype::client::WalletChangeReading>
RecordingAccountLink::purchase() const {
  return purchaseRead;
}

std::optional<rtype::client::WalletChangeReading>
RecordingAccountLink::exchange() const {
  return exchangeRead;
}
