#include "FixedMasterAddress.hpp"
#include <string>
#include <utility>

FixedMasterAddress::FixedMasterAddress(std::string url)
    : url_(std::move(url)) {}

std::string FixedMasterAddress::url() const { return url_; }
