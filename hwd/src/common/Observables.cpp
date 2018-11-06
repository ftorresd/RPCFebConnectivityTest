#include "rpct/hwd/Observables.h"

#include <stdexcept>
#include <sstream>

#include "rpct/hwd/DeviceObservables.h"

#include "rpct/hwd/Parameter.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/Device.h"

namespace rpct {
namespace hwd {

Observables const Observables::default_ = Observables();

Observables::Observables()
    : deviceobservable_id_(0)
{}

bool Observables::hasDeviceObservables(Device const & _device) const
{
    std::map<integer_type, integer_type>::const_iterator _deviceobservables
        = device_deviceobservables_.find(_device.getId());
    return (_deviceobservables != device_deviceobservables_.end());
}

DeviceObservables const & Observables::getDeviceObservables(Device const & _device) const
{
    std::map<integer_type, integer_type>::const_iterator _deviceobservables
        = device_deviceobservables_.find(_device.getId());
    if (_deviceobservables != device_deviceobservables_.end())
        return deviceobservables_.find(_deviceobservables->second)->second;
    else
        {
            std::ostringstream _errormsg;
            _errormsg << "No DeviceObservables available for Device " << _device.getDeviceType().getName()
                      << " with id " << _device.getId();
            throw std::out_of_range(_errormsg.str());
        }
}

DeviceObservables & Observables::addDeviceObservables(DeviceType const & _devicetype)
{
    ++deviceobservable_id_;
    return deviceobservables_.insert(std::pair<integer_type
                                     , DeviceObservables>(deviceobservable_id_
                                                          , DeviceObservables(deviceobservable_id_, _devicetype))).first->second;
}

void Observables::addDeviceObservables(Device const & _device
                                       , DeviceObservables const & _deviceobservables)
{
    if (deviceobservables_.find(_deviceobservables.getId()) == deviceobservables_.end())
        throw std::logic_error("Attempt to link Device to DeviceObservables in wrong Observables");
    if (_device.getDeviceType() == _deviceobservables.getDeviceType())
        device_deviceobservables_.insert(std::pair<integer_type, integer_type>(_device.getId()
                                                                               , _deviceobservables.getId()));
    else
        throw std::logic_error("Attempt to link Device and DeviceObservables with mismatching DeviceType");
}

} // namespace hwd
} // namespace rpct
