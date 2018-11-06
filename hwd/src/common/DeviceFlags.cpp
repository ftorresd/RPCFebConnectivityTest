#include "rpct/hwd/DeviceFlags.h"

namespace rpct {
namespace hwd {

std::string const DeviceFlags::parametertype_name_ = std::string("DeviceFlags");

DeviceFlags const DeviceFlags::default_ = DeviceFlags();

DeviceFlags::DeviceFlags(unsigned char _flags)
    : flags_(_flags)
{}

DeviceFlags::~DeviceFlags()
{}

} // namespace hwd
} // namespace rpct
