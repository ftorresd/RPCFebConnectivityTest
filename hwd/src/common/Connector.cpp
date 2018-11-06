#include "rpct/hwd/Connector.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/Service.h"

namespace rpct {
namespace hwd {

Connector const Connector::default_ = Connector();

Connector::Connector(DeviceType const & _devicetype
                     , integer_type _id
                     , Service const & _service
                     , integer_type _position
                     , Direction _direction)
    : devicetype_(&_devicetype)
    , id_(_id)
    , service_(&_service)
    , position_(_position)
    , direction_(_direction)
{}

Connector::Connector()
    : devicetype_(&(DeviceType::default_))
    , id_(-1)
    , service_(&(Service::default_))
    , position_(0)
    , direction_(bidirectional_)
{}

Connector::~Connector()
{}

} // namespace hwd
} // namespace rpct
