#include "rpct/hwd/Device.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/Configuration.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"
#include "rpct/hwd/Observables.h"

#include "rpct/hwd/System.h"

namespace rpct {
namespace hwd {

Device::Device(System & _system
               , integer_type _id
               , DeviceType const & _devicetype
               , DeviceConfiguration const &)
    : system_(&_system)
    , id_(_id)
    , devicetype_(&_devicetype)
{}

Device::Device()
    : system_(0)
    , id_(-1)
    , devicetype_(&(DeviceType::default_))
{}

Device::~Device()
{}

void Device::configure(Configuration const & _configuration, DeviceFlagsMask const & _mask)
{
    if (_configuration.hasDeviceConfiguration(*this))
        configure(_configuration.getDeviceConfiguration(*this), _mask);

    iconfigure(_configuration, _mask);
}

void Device::configure(DeviceConfiguration const & _deviceconfiguration, DeviceFlagsMask const & _mask)
{
    if (!(_mask.matches(getDeviceFlags())))
        return;

    if (_deviceconfiguration.hasParameterSetting(getDeviceType().getDeviceStateParameter()))
        {
            unsigned char _state;
            _deviceconfiguration.getParameterSetting(getDeviceType().getDeviceStateParameter(), _state);
            try {
                getDeviceState().setState(_state);
            } catch (...) {}
        }

    if (_deviceconfiguration.hasParameterSetting(getDeviceType().getDeviceFlagsParameter()))
        {
            unsigned char _flags;
            _deviceconfiguration.getParameterSetting(getDeviceType().getDeviceFlagsParameter(), _flags);
            try {
                getDeviceFlags().setFlags(_flags);
            } catch (...) {}
        }

    iconfigure(_deviceconfiguration, _mask);

    getSystem().publish(*this, _deviceconfiguration);

    getSystem().publish(*this, getDeviceType().getDeviceStateParameter(), getDeviceState().getState());
    getSystem().publish(*this, getDeviceType().getDeviceFlagsParameter(), getDeviceFlags().getFlags());
}

void Device::configure(StandaloneDeviceConfiguration const & _deviceconfiguration
                       , DeviceFlagsMask const & _mask)
{
    configure(dynamic_cast<DeviceConfiguration const &>(_deviceconfiguration), _mask);
}

void Device::monitor(DeviceFlagsMask const & _mask)
{
    monitor(getDeviceType().getDeviceObservables(), _mask);

    imonitor(_mask);
}

void Device::monitor(Observables const & _observables, DeviceFlagsMask const & _mask)
{
    if (_observables.hasDeviceObservables(*this))
        monitor(_observables.getDeviceObservables(*this), _mask);

    imonitor(_observables, _mask);
}

void Device::monitor(DeviceObservables const & _deviceobservables, DeviceFlagsMask const & _mask)
{
    if (!(_mask.matches(getDeviceFlags())))
        return;

    imonitor(_deviceobservables, _mask);

    getSystem().publish(*this, getDeviceType().getDeviceStateParameter(), getDeviceState().getState());
    getSystem().publish(*this, getDeviceType().getDeviceFlagsParameter(), getDeviceFlags().getFlags());
}

void Device::assume(Configuration const & _configuration)
{
    if (_configuration.hasDeviceConfiguration(*this))
        assume(_configuration.getDeviceConfiguration(*this));

    iassume(_configuration);
}

void Device::assume(DeviceConfiguration const & _deviceconfiguration)
{
    if (_deviceconfiguration.hasParameterSetting(getDeviceType().getDeviceStateParameter()))
        {
            unsigned char _state;
            _deviceconfiguration.getParameterSetting(getDeviceType().getDeviceStateParameter(), _state);
            try {
                getDeviceState().setState(_state);
            } catch (...) {}
        }

    if (_deviceconfiguration.hasParameterSetting(getDeviceType().getDeviceFlagsParameter()))
        {
            unsigned char _flags;
            _deviceconfiguration.getParameterSetting(getDeviceType().getDeviceFlagsParameter(), _flags);
            try {
                getDeviceFlags().setFlags(_flags);
            } catch (...) {}
        }

    iassume(_deviceconfiguration);
}

void Device::assume(StandaloneDeviceConfiguration const & _deviceconfiguration)
{
    assume(dynamic_cast<DeviceConfiguration const &>(_deviceconfiguration));
}

} // namespace hwd
} // namespace rpct
