#include "MasterRequest.hpp"
#include <httplib.h>
#include <string>
#include <utility>
#include "MasterHttpConstants.hpp"
#include "MasterRoute.hpp"
#include "MasterRoutePath.hpp"

namespace rtype::network {

httplib::Request masterRequestOf(MasterRoute route, std::string path) {
  httplib::Request request;
  request.method = std::string{methodNameOf(route.method)};
  request.path = std::move(path);
  return request;
}

httplib::Request jsonRequestOf(MasterRoute route, std::string path,
                               std::string body) {
  httplib::Request request = masterRequestOf(route, std::move(path));
  request.body = std::move(body);
  request.set_header(std::string{CONTENT_TYPE_HEADER},
                     std::string{JSON_CONTENT_TYPE});
  return request;
}

}  // namespace rtype::network
