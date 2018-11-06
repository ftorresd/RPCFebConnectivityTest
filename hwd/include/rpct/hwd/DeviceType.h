#ifndef rpct_hwd_DeviceType_h
#define rpct_hwd_DeviceType_h

#include <string>
#include <set>

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/DeviceObservables.h"

#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Connector.h"

namespace rpct {
namespace hwd {

class DeviceType
{
public:
    static DeviceType const default_;

protected:
    struct ParameterCompare
    {
        bool operator()(Parameter const * _lhs, Parameter const * _rhs) const;
    };

    struct ConnectorCompare
    {
        bool operator()(Connector const * _lhs, Connector const * _rhs) const;
    };

    struct ParameterCompareId
    {
        bool operator()(Parameter const & _lhs, Parameter const & _rhs) const;
        bool operator()(Parameter const & _lhs, integer_type const & _rhs) const;
        bool operator()(integer_type const & _lhs, Parameter const & _rhs) const;
    };
    static ParameterCompareId const parameter_compare_id_;

    struct ConnectorCompareId
    {
        bool operator()(Connector const & _lhs, Connector const & _rhs) const;
        bool operator()(Connector const & _lhs, integer_type const & _rhs) const;
        bool operator()(integer_type const & _lhs, Connector const & _rhs) const;
    };
    static ConnectorCompareId const connector_compare_id_;

public:
    DeviceType(System & _system
               , integer_type _id
               , std::string const & _name);
    DeviceType();
    virtual ~DeviceType();

    System const & getSystem() const;
    System & getSystem();

    integer_type getId() const;
    std::string const & getName() const;

    DeviceObservables const & getDeviceObservables() const;

    std::set<Parameter> const & getParameters() const;
    std::set<Connector> const & getConnectors() const;

    Parameter const & registerParameter(ParameterType const & _parametertype
                                        , integer_type _position = 0
                                        , unsigned char _properties = 0x0);
    Parameter const & getParameter(integer_type _id) const;
    Parameter const & getParameter(integer_type _id);
    Parameter const & getParameter(ParameterType const & _parametertype
                                   , integer_type _position = 0) const;
    Parameter const & getParameter(ParameterType const & _parametertype
                                   , integer_type _position = 0);
    Parameter const & getParameter(std::string const & _parametertype_name
                                   , integer_type _position = 0) const;
    Parameter const & getParameter(std::string const & _parametertype_name
                                   , integer_type _position = 0);

    Parameter const & getDeviceStateParameter() const;
    Parameter const & getDeviceFlagsParameter() const;

    Connector const & registerConnector(Service const & _service
                                        , integer_type _position = 0
                                        , Direction _direction = bidirectional_);

    Connector const & getConnector(integer_type _id) const;
    Connector const & getConnector(integer_type _id);
    Connector const & getConnector(Service const & _service
                                   , integer_type _position = 0
                                   , Direction _direction = bidirectional_) const;
    Connector const & getConnector(Service const & _service
                                   , integer_type _position = 0
                                   , Direction _direction = bidirectional_);
    Connector const & getConnector(std::string const & _service_name
                                   , integer_type _position = 0
                                   , Direction _direction = bidirectional_) const;
    Connector const & getConnector(std::string const & _service_name
                                   , integer_type _position = 0
                                   , Direction _direction = bidirectional_);

    bool operator< (DeviceType const & _devicetype) const;
    bool operator==(DeviceType const & _devicetype) const;
    bool operator!=(DeviceType const & _devicetype) const;

protected:
    friend class HardwareStorage;

    virtual Parameter const & addParameter(integer_type _id
                                           , ParameterType const & _parametertype
                                           , integer_type _position = 0
                                           , unsigned char _properties = 0x0);

    virtual Connector const & addConnector(integer_type _id
                                           , Service const & _service
                                           , integer_type _position = 0
                                           , Direction _direction = bidirectional_);
protected:
    System * system_;
    integer_type id_;
    std::string name_;

    std::set<Parameter> id_parameters_;
    std::set<Parameter const *, ParameterCompare> parameters_;

    std::set<Connector> id_connectors_;
    std::set<Connector const *, ConnectorCompare> connectors_;

    DeviceObservables deviceobservables_;

    Parameter const * state_parameter_;
    Parameter const * flags_parameter_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/DeviceType-inl.h"

#endif // rpct_hwd_DeviceType_h
