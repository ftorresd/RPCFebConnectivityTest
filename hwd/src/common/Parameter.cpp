#include "rpct/hwd/Parameter.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/ParameterType.h"

namespace rpct {
namespace hwd {

Parameter const Parameter::default_ = Parameter();

Parameter::Parameter(DeviceType const & _devicetype
                     , integer_type _id
                     , ParameterType const & _parametertype
                     , integer_type _position
                     , unsigned char _properties)
    : devicetype_(&_devicetype)
    , id_(_id)
    , parametertype_(&_parametertype)
    , position_(_position)
    , properties_(_properties)
{}

Parameter::Parameter()
    : devicetype_(&(DeviceType::default_))
    , id_(-1)
    , parametertype_(&(ParameterType::default_))
    , position_(0)
    , properties_(0x0)
{}

Parameter::~Parameter()
{}

} // namespace hwd
} // namespace rpct
