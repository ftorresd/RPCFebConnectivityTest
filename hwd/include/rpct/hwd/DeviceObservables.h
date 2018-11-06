#ifndef rpct_hwd_DeviceObservables_h
#define rpct_hwd_DeviceObservables_h

#include <set>

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class DeviceObservables
{
public:
    static DeviceObservables const default_;

public:
    DeviceObservables(DeviceType const & _devicetype);
    DeviceObservables(integer_type _id
                      , DeviceType const & _devicetype);
    DeviceObservables();

    integer_type getId() const;
    DeviceType const & getDeviceType() const;

    bool hasParameter(Parameter const & _parameter) const;

    void addParameter(Parameter const & _parameter);
    void removeParameter(Parameter const & _parameter);

    bool operator< (DeviceObservables const & _deviceobservables) const;
    bool operator==(DeviceObservables const & _deviceobservables) const;
    bool operator!=(DeviceObservables const & _deviceobservables) const;

protected:
    integer_type id_;
    DeviceType const * devicetype_;
    std::set<integer_type> parameters_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/DeviceObservables-inl.h"

#endif // rpct_hwd_DeviceObservables_h
