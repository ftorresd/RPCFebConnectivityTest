#include "rpct/hwd/StandaloneDeviceConfiguration.h"

namespace rpct {
namespace hwd {

StandaloneDeviceConfiguration const StandaloneDeviceConfiguration::default_ = StandaloneDeviceConfiguration();

StandaloneDeviceConfiguration::StandaloneDeviceConfiguration(System & _system
                                                             , integer_type _id
                                                             , DeviceType const & _devicetype)
    : Configuration(_system, _id, "Standalone")
    , DeviceConfiguration(*this, _id, _devicetype)
{}

StandaloneDeviceConfiguration::StandaloneDeviceConfiguration()
    : Configuration()
    , DeviceConfiguration(*this, -1, DeviceType::default_)
{
    name_ = std::string("Standalone");
}

} // namespace hwd
} // namespace rpct
