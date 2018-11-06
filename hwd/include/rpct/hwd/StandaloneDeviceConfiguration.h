#ifndef rpct_hwd_StandaloneDeviceConfiguration_h
#define rpct_hwd_StandaloneDeviceConfiguration_h

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/Configuration.h"

namespace rpct {
namespace hwd {

class StandaloneDeviceConfiguration
    : protected Configuration
    , public DeviceConfiguration
{
public:
    static StandaloneDeviceConfiguration const default_;
public:
    StandaloneDeviceConfiguration(System & _system
                                  , integer_type _id
                                  , DeviceType const & _devicetype);
    StandaloneDeviceConfiguration();
};

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_StandaloneDeviceConfiguration_h
