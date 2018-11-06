#ifndef rpct_hwd_Parameter_h
#define rpct_hwd_Parameter_h

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class Parameter
{
public:
    static Parameter const default_;

    static unsigned char const is_property_     = 0x01;
    static unsigned char const is_observable_   = 0x02;
    static unsigned char const is_configurable_ = 0x04;

public:
    Parameter(DeviceType const & _devicetype
              , integer_type _id
              , ParameterType const & _parametertype
              , integer_type _position = 0
              , unsigned char _properties = 0x0);
    Parameter();
    virtual ~Parameter();

    DeviceType const & getDeviceType() const;
    integer_type getId() const;
    ParameterType const & getParameterType() const;
    DataType const & getDataType() const;

    integer_type getPosition() const;

    bool isProperty() const;
    bool isObservable() const;
    bool isConfigurable() const;

    bool operator< (Parameter const & _parameter) const;
    bool operator==(Parameter const & _parameter) const;
    bool operator!=(Parameter const & _parameter) const;

protected:
    DeviceType const * devicetype_;
    integer_type id_;
    ParameterType const * parametertype_;

    integer_type position_;
    unsigned char properties_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/Parameter-inl.h"

#endif // rpct_hwd_Parameter_h
