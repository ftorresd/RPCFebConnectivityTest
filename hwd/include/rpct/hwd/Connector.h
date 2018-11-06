#ifndef rpct_hwd_Connector_h
#define rpct_hwd_Connector_h

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class Connector
{
public:
    static Connector const default_;

public:
    Connector(DeviceType const & _devicetype
              , integer_type _id
              , Service const & _service
              , integer_type _position = 0
              , Direction _direction = bidirectional_);
    Connector();
    virtual ~Connector();

    DeviceType const & getDeviceType() const;
    integer_type getId() const;
    Service const & getService() const;

    integer_type getPosition() const;
    Direction getDirection() const;

    bool operator< (Connector const & _connector) const;
    bool operator==(Connector const & _connector) const;
    bool operator!=(Connector const & _connector) const;

protected:
    DeviceType const * devicetype_;
    integer_type id_;
    Service const * service_;

    integer_type position_;
    Direction direction_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/Connector-inl.h"

#endif // rpct_hwd_Connector_h
