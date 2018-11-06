#ifndef rpct_hwd_DeviceObservables_inl_h
#define rpct_hwd_DeviceObservables_inl_h

#include "rpct/hwd/DeviceObservables.h"

namespace rpct {
namespace hwd {

inline integer_type DeviceObservables::getId() const
{
    return id_;
}

inline DeviceType const & DeviceObservables::getDeviceType() const
{
    if (devicetype_)
        return *devicetype_;
    throw std::logic_error("Attempt to get DeviceType of an incomplete DeviceObservables");
}

inline bool DeviceObservables::operator<(DeviceObservables const & _deviceobservables) const
{
    return (id_ < _deviceobservables.id_);
}

inline bool DeviceObservables::operator==(DeviceObservables const & _deviceobservables) const
{
    return (id_ == _deviceobservables.id_);
}

inline bool DeviceObservables::operator!=(DeviceObservables const & _deviceobservables) const
{
    return (id_ != _deviceobservables.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_DeviceObservables_inl_h
