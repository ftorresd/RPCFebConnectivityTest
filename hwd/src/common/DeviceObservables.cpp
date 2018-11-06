#include "rpct/hwd/DeviceObservables.h"

#include <stdexcept>

#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/DeviceType.h"

namespace rpct {
namespace hwd {

DeviceObservables const DeviceObservables::default_ = DeviceObservables();

DeviceObservables::DeviceObservables(DeviceType const & _devicetype)
    : id_(-1)
    , devicetype_(&_devicetype)
{}

DeviceObservables::DeviceObservables(integer_type _id
                                     , DeviceType const & _devicetype)
    : id_(_id)
    , devicetype_(&_devicetype)
{}

DeviceObservables::DeviceObservables()
    : id_(-1)
    , devicetype_(&(DeviceType::default_))
{}

bool DeviceObservables::hasParameter(Parameter const & _parameter) const
{
    return (parameters_.find(_parameter.getId()) != parameters_.end());
}

void DeviceObservables::addParameter(Parameter const & _parameter)
{
    if (_parameter.isObservable() && _parameter.getDeviceType() == getDeviceType())
        parameters_.insert(_parameter.getId());
    else
        throw std::logic_error("Attempt to add parameter that is not an observable for DeviceObservables DeviceType");
}

void DeviceObservables::removeParameter(Parameter const & _parameter)
{
    parameters_.erase(_parameter.getId());
}

} // namespace hwd
} // namespace rpct
