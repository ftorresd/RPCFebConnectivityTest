#include "rpct/hwd/DeviceFlagsMask.h"

namespace rpct {
namespace hwd {

DeviceFlagsMask const DeviceFlagsMask::default_      = DeviceFlagsMask();
DeviceFlagsMask const DeviceFlagsMask::any_          = DeviceFlagsMask(0x00, 0x00);
DeviceFlagsMask const DeviceFlagsMask::not_disabled_ = DeviceFlagsMask(0x00, (0x01 << DeviceFlags::disabled_));

DeviceFlagsMask::DeviceFlagsMask(unsigned char _flags, unsigned char _mask)
    : flags_(_flags)
    , mask_(_mask)
{}

DeviceFlagsMask::~DeviceFlagsMask()
{}

} // namespace hwd
} // namespace rpct
