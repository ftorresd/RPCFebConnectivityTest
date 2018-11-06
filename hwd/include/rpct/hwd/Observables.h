#ifndef rpct_hwd_Observables_h
#define rpct_hwd_Observables_h

#include <map>

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/DeviceObservables.h"

namespace rpct {
namespace hwd {

class Observables
{
public:
    static Observables const default_;

public:
    Observables();

    bool hasDeviceObservables(Device const & _device) const;
    DeviceObservables const & getDeviceObservables(Device const & _device) const;

    DeviceObservables & addDeviceObservables(DeviceType const & _devicetype);
    void addDeviceObservables(Device const & _device
                              , DeviceObservables const & _deviceobservables);

protected:
    integer_type deviceobservable_id_;
    std::map<integer_type, integer_type> device_deviceobservables_;
    std::map<integer_type, DeviceObservables> deviceobservables_;
};

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Observables_h
