#ifndef rpct_hwd_Parameter_inl_h
#define rpct_hwd_Parameter_inl_h

#include "rpct/hwd/Parameter.h"

#include <stdexcept>

#include "rpct/hwd/ParameterType.h"

namespace rpct {
namespace hwd {

inline DeviceType const & Parameter::getDeviceType() const
{
    if (devicetype_)
        return *devicetype_;
    throw std::logic_error("Attempt to get DeviceType of an incomplete Parameter");
}

inline integer_type Parameter::getId() const
{
    return id_;
}

inline ParameterType const & Parameter::getParameterType() const
{
    if (parametertype_)
        return *parametertype_;
    throw std::logic_error("Attempt to get ParameterType of an incomplete Parameter");
}

inline DataType const & Parameter::getDataType() const
{
    return getParameterType().getDataType();
}

inline integer_type Parameter::getPosition() const
{
    return position_;
}

inline bool Parameter::isProperty() const
{
    return (properties_ & is_property_);
}

inline bool Parameter::isObservable() const
{
    return (properties_ & is_observable_);
}

inline bool Parameter::isConfigurable() const
{
    return (properties_ & is_configurable_);
}

inline bool Parameter::operator<(Parameter const & _parameter) const
{
    return (id_ < _parameter.id_);
}

inline bool Parameter::operator==(Parameter const & _parameter) const
{
    return (id_ == _parameter.id_);
}

inline bool Parameter::operator!=(Parameter const & _parameter) const
{
    return (id_ != _parameter.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Parameter_inl_h
