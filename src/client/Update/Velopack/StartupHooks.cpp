#include "StartupHooks.hpp"
#include <Velopack.hpp>

namespace rtype::client {

void runVelopackStartup() { Velopack::VelopackApp::Build().Run(); }

}  // namespace rtype::client
