#ifndef rpct_hwd_DeviceType_inl_h
#define rpct_hwd_DeviceType_inl_h

#include "rpct/hwd/DeviceType.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline bool DeviceType::ParameterCompareId::operator()(Parameter const & _lhs, Parameter const & _rhs) const
{
    return _lhs.getId() < _rhs.getId();
}

inline bool DeviceType::ParameterCompareId::operator()(Parameter const & _lhs, integer_type const & _rhs) const
{
    return _lhs.getId() < _rhs;
}

inline bool DeviceType::ParameterCompareId::operator()(integer_type const & _lhs, Parameter const & _rhs) const
{
    return _lhs < _rhs.getId();
}

inline bool DeviceType::ConnectorCompareId::operator()(Connector const & _lhs, Connector const & _rhs) const
{
    return _lhs.getId() < _rhs.getId();
}

inline bool DeviceType::ConnectorCompareId::operator()(Connector const & _lhs, integer_type const & _rhs) const
{
    return _lhs.getId() < _rhs;
}

inline bool DeviceType::ConnectorCompareId::operator()(integer_type const & _lhs, Connector const & _rhs) const
{
    return _lhs < _rhs.getId();
}

inline System const & DeviceType::getSystem() const
{
    if (system_)
        return *system_;
    throw std::logic_error("Attempt to get System of an incomplete DeviceType");
}

inline System & DeviceType::getSystem()
{
    if (system_)
        return *system_;
    throw std::logic_error("Attempt to get System of an incomplete DeviceType");
}

inline integer_type DeviceType::getId() const
{
    return id_;
}

inline std::string const & DeviceType::getName() const
{
    return name_;
}

inline DeviceObservables const & DeviceType::getDeviceObservables() const
{
    return deviceobservables_;
}

inline std::set<Parameter> const & DeviceType::getParameters() const
{
    return id_parameters_;
}

inline std::set<Connector> const & DeviceType::getConnectors() const
{
    return id_connectors_;
}

inline Parameter const & DeviceType::getDeviceStateParameter() const
{
    if (state_parameter_)
        return *state_parameter_;
    throw std::logic_error("Attempt to get DeviceState Parameter of an incomplete DeviceType");
}

inline Parameter const & DeviceType::getDeviceFlagsParameter() const
{
    if (flags_parameter_)
        return *flags_parameter_;
    throw std::logic_error("Attempt to get DeviceFlags Parameter of an incomplete DeviceType");
}

inline bool DeviceType::operator<(DeviceType const & _devicetype) const
{
    return (id_ < _devicetype.id_);
}

inline bool DeviceType::operator==(DeviceType const & _devicetype) const
{
    return (id_ == _devicetype.id_);
}

inline bool DeviceType::operator!=(DeviceType const & _devicetype) const
{
    return (id_ != _devicetype.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_DeviceType_inl_h
