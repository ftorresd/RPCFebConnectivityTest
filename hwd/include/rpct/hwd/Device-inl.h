#ifndef rpct_hwd_Device_inl_h
#define rpct_hwd_Device_inl_h

#include "rpct/hwd/Device.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline void Device::initialise()
{}

inline System & Device::getSystem()
{
    if (system_)
        return *system_;
    throw std::logic_error("Attempt to get System of an incomplete Device");
}

inline System const & Device::getSystem() const
{
    if (system_)
        return *system_;
    throw std::logic_error("Attempt to get System of an incomplete Device");
}

inline integer_type Device::getId() const
{
    return id_;
}

inline DeviceType const & Device::getDeviceType() const
{
    if (devicetype_)
        return *devicetype_;
    throw std::logic_error("Attempt to get DeviceType of an incomplete Device");
}

inline State & Device::getDeviceState()
{
    return state_;
}

inline State const & Device::getDeviceState() const
{
    return state_;
}

inline DeviceFlags & Device::getDeviceFlags()
{
    return flags_;
}

inline DeviceFlags const & Device::getDeviceFlags() const
{
    return flags_;
}

inline void Device::configure(DeviceFlagsMask const & _mask)
{
    iconfigure(_mask);
}

inline bool Device::operator<(Device const & _device) const
{
    return getId() < _device.getId();
}

inline bool Device::operator==(Device const & _device) const
{
    return getId() == _device.getId();
}

inline bool Device::operator!=(Device const & _device) const
{
    return getId() != _device.getId();
}

inline void Device::addConnection(Connection &)
{}

inline void Device::iconfigure(DeviceFlagsMask const &)
{}

inline void Device::iconfigure(Configuration const &, DeviceFlagsMask const &)
{}

inline void Device::iconfigure(DeviceConfiguration const &, DeviceFlagsMask const &)
{}

inline void Device::imonitor(DeviceFlagsMask const &)
{}

inline void Device::imonitor(Observables const &, DeviceFlagsMask const &)
{}

inline void Device::imonitor(DeviceObservables const &, DeviceFlagsMask const &)
{}

inline void Device::iassume(Configuration const &)
{}

inline void Device::iassume(DeviceConfiguration const &)
{}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Device_inl_h
