#ifndef rpct_hwd_Connector_inl_h
#define rpct_hwd_Connector_inl_h

#include "rpct/hwd/Connector.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline DeviceType const & Connector::getDeviceType() const
{
    if (devicetype_)
        return *devicetype_;
    throw std::logic_error("Attempt to get DeviceType of an incomplete Connector");
}

inline integer_type Connector::getId() const
{
    return id_;
}

inline Service const & Connector::getService() const
{
    if (service_)
        return *service_;
    throw std::logic_error("Attempt to get Service of an incomplete Connector");
}

inline integer_type Connector::getPosition() const
{
    return position_;
}

inline Direction Connector::getDirection() const
{
    return direction_;
}

inline bool Connector::operator<(Connector const & _connector) const
{
    return (id_ < _connector.id_);
}

inline bool Connector::operator==(Connector const & _connector) const
{
    return (id_ == _connector.id_);
}

inline bool Connector::operator!=(Connector const & _connector) const
{
    return (id_ != _connector.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Connector_inl_h
