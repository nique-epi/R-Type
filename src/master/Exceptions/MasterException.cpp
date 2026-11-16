#include "MasterException.hpp"
#include <stdexcept>
#include <string>
#include <string_view>

namespace rtype::master {

MasterException::MasterException(const std::string& message)
    : std::runtime_error(message) {}

DatabaseClosedException::DatabaseClosedException()
    : MasterException("the database of the master is not open") {}

MissingPasswordKeyException::MissingPasswordKeyException(
    std::string_view variable)
    : MasterException(std::string{variable} +
                      " is not set: give the master the key of the passwords "
                      "of its accounts (`openssl rand -hex 32`), or turn them "
                      "off with `module accounts off` in master.cfg, after "
                      "the modules that need them, such as "
                      "`module casino off` and `module wallet off`") {}

InvalidPasswordKeyException::InvalidPasswordKeyException(
    std::string_view variable)
    : MasterException(std::string{variable} +
                      " holds no key: 64 hexadecimal digits are expected, as "
                      "`openssl rand -hex 32` draws them") {}

CasinoStakeRefusedException::CasinoStakeRefusedException()
    : MasterException("the wallet refused the stake of the bets") {}

}  // namespace rtype::master
