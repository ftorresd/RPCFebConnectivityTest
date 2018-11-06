#ifndef rpct_hwd_Device_h
#define rpct_hwd_Device_h

#include "rpct/hwd/fwd.h"

#include "rpct/hwd/State.h"
#include "rpct/hwd/DeviceFlags.h"
#include "rpct/hwd/DeviceFlagsMask.h"

namespace rpct {
namespace hwd {

class Device
{
public:
    Device(System & _system
           , integer_type _id
           , DeviceType const & _devicetype
           , DeviceConfiguration const & _properties);
    Device();
    virtual ~Device();

    virtual void initialise();

    System & getSystem();
    System const & getSystem() const;
    integer_type getId() const;
    DeviceType const & getDeviceType() const;

    State & getDeviceState();
    State const & getDeviceState() const;

    DeviceFlags & getDeviceFlags();
    DeviceFlags const & getDeviceFlags() const;

    void configure(DeviceFlagsMask const & _mask = DeviceFlagsMask::not_disabled_);
    void configure(Configuration const & _configuration
                   , DeviceFlagsMask const & _mask = DeviceFlagsMask::not_disabled_);
    void configure(DeviceConfiguration const & _deviceconfiguration
                   , DeviceFlagsMask const & _mask = DeviceFlagsMask::not_disabled_);
    void configure(StandaloneDeviceConfiguration const & _deviceconfiguration
                   , DeviceFlagsMask const & _mask = DeviceFlagsMask::not_disabled_);

    void monitor(DeviceFlagsMask const & _mask = DeviceFlagsMask::not_disabled_);
    void monitor(Observables const & _observables
                 , DeviceFlagsMask const & _mask = DeviceFlagsMask::not_disabled_);
    void monitor(DeviceObservables const & _deviceobservables
                 , DeviceFlagsMask const & _mask = DeviceFlagsMask::not_disabled_);

    void assume(Configuration const & _configuration);
    void assume(DeviceConfiguration const & _deviceconfiguration);
    void assume(StandaloneDeviceConfiguration const & _deviceconfiguration);

    bool operator< (Device const & _device) const;
    bool operator==(Device const & _device) const;
    bool operator!=(Device const & _device) const;

    virtual void addConnection(Connection & _connection);

protected:
    virtual void iconfigure(DeviceFlagsMask const & _mask);
    virtual void iconfigure(Configuration const & _configuration, DeviceFlagsMask const & _mask);
    virtual void iconfigure(DeviceConfiguration const & _deviceconfiguration, DeviceFlagsMask const & _mask);

    virtual void imonitor(DeviceFlagsMask const & _mask);
    virtual void imonitor(Observables const & _observables, DeviceFlagsMask const & _mask);
    virtual void imonitor(DeviceObservables const & _deviceobservables, DeviceFlagsMask const & _mask);

    virtual void iassume(Configuration const & _configuration);
    virtual void iassume(DeviceConfiguration const & _deviceconfiguration);

protected:
    System * system_;
    integer_type id_;
    DeviceType const * devicetype_;

    State state_;
    DeviceFlags flags_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/Device-inl.h"

#endif // rpct_hwd_Device_h
