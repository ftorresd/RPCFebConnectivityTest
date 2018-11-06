#include "rpct/hwd/DeviceType.h"

#include <algorithm>
#include <stdexcept>
#include <sstream>

#include "rpct/hwd/State.h"
#include "rpct/hwd/DeviceFlags.h"

#include "rpct/hwd/System.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/HardwareStorage.h"

namespace rpct {
namespace hwd {

DeviceType const DeviceType::default_ = DeviceType();

DeviceType::ParameterCompareId const DeviceType::parameter_compare_id_ = DeviceType::ParameterCompareId();
DeviceType::ConnectorCompareId const DeviceType::connector_compare_id_ = DeviceType::ConnectorCompareId();

bool DeviceType::ParameterCompare::operator()(Parameter const * _lhs, Parameter const * _rhs) const
{
    return (_lhs->getParameterType() < _rhs->getParameterType()
            || (_lhs->getParameterType() == _rhs->getParameterType()
                && _lhs->getPosition() < _rhs->getPosition()
                )
            );
}

bool DeviceType::ConnectorCompare::operator()(Connector const * _lhs, Connector const * _rhs) const
{
    return (_lhs->getService() < _rhs->getService()
            || (_lhs->getService() == _rhs->getService()
                && (_lhs->getPosition() < _rhs->getPosition()
                    || (_lhs->getPosition() == _rhs->getPosition()
                        && _lhs->getDirection() < _rhs->getDirection()
                        )
                    )
                )
            );
}

DeviceType::DeviceType(System & _system
                       , integer_type _id
                       , std::string const & _name)
    : system_(&_system)
    , id_(_id)
    , name_(_name)
    , deviceobservables_(*this)
    , state_parameter_(0)
    , flags_parameter_(0)
{
    state_parameter_ = &(registerParameter(_system.getParameterType(State::parametertype_name_), 0
                                           , Parameter::is_observable_ | Parameter::is_configurable_));
    flags_parameter_ = &(registerParameter(_system.getParameterType(DeviceFlags::parametertype_name_), 0
                                           , Parameter::is_property_ | Parameter::is_observable_ | Parameter::is_configurable_));
}

DeviceType::DeviceType()
    : system_(0)
    , id_(-1)
    , name_("DefaultDevice")
    , deviceobservables_(*this)
    , state_parameter_(0)
    , flags_parameter_(0)
{}

DeviceType::~DeviceType()
{}

Parameter const & DeviceType::registerParameter(ParameterType const & _parametertype
                                                , integer_type _position
                                                , unsigned char _properties)
{
    return getSystem().getHardwareStorage().registerParameter(*this, _parametertype
                                                              , _position
                                                              , _properties);
}

Parameter const & DeviceType::getParameter(integer_type _id) const
{
    std::set<Parameter>::const_iterator _it
        = std::lower_bound(id_parameters_.begin(), id_parameters_.end(), _id, parameter_compare_id_);
    if (_it != id_parameters_.end() && !parameter_compare_id_(_id, *_it))
        return *_it;
    else
        {
            std::ostringstream _errormsg;
            _errormsg << "Parameter " << _id << " not present in DeviceType " << name_;
            throw std::out_of_range(_errormsg.str());
        }
}

Parameter const & DeviceType::getParameter(integer_type _id)
{
    std::set<Parameter>::const_iterator _it
        = std::lower_bound(id_parameters_.begin(), id_parameters_.end(), _id, parameter_compare_id_);
    if (_it != id_parameters_.end() && !parameter_compare_id_(_id, *_it))
        return *_it;
    else
        return getSystem().getHardwareStorage().getParameter(*this, _id);
}

Parameter const & DeviceType::getParameter(ParameterType const & _parametertype
                                           , integer_type _position) const
{
    Parameter _temp(*this, -1, _parametertype, _position);
    std::set<Parameter const *, ParameterCompare>::const_iterator _parameter = parameters_.find(&_temp);
    if (_parameter != parameters_.end())
        return *(*_parameter);
    std::ostringstream _errormsg;
    _errormsg << "Could not find Parameter with ParameterType " << _parametertype.getName()
              << " and position " << _position << " for DeviceType " << name_;
    throw std::out_of_range(_errormsg.str());
}

Parameter const & DeviceType::getParameter(ParameterType const & _parametertype
                                           , integer_type _position)
{
    Parameter _temp(*this, -1, _parametertype, _position);
    std::set<Parameter const *, ParameterCompare>::const_iterator _parameter = parameters_.find(&_temp);
    if (_parameter != parameters_.end())
        return *(*_parameter);
    return getSystem().getHardwareStorage().getParameter(*this, _parametertype, _position);
}

Parameter const & DeviceType::getParameter(std::string const & _parametertype_name
                                           , integer_type _position) const
{
    return getParameter(getSystem().getParameterType(_parametertype_name), _position);
}

Parameter const & DeviceType::getParameter(std::string const & _parametertype_name
                                           , integer_type _position)
{
    return getParameter(getSystem().getParameterType(_parametertype_name), _position);
}

Connector const & DeviceType::registerConnector(Service const & _service
                                                , integer_type _position
                                                , Direction _direction)
{
    return getSystem().getHardwareStorage().registerConnector(*this, _service
                                                              , _position
                                                              , _direction);
}

Connector const & DeviceType::getConnector(integer_type _id) const
{
    std::set<Connector>::const_iterator _it
        = std::lower_bound(id_connectors_.begin(), id_connectors_.end(), _id, connector_compare_id_);
    if (_it != id_connectors_.end() && !connector_compare_id_(_id, *_it))
        return *_it;
    else
        {
            std::ostringstream _errormsg;
            _errormsg << "Connector " << _id << " not present in DeviceType " << name_;
            throw std::out_of_range(_errormsg.str());
        }
}

Connector const & DeviceType::getConnector(integer_type _id)
{
    std::set<Connector>::const_iterator _it
        = std::lower_bound(id_connectors_.begin(), id_connectors_.end(), _id, connector_compare_id_);
    if (_it != id_connectors_.end() && !connector_compare_id_(_id, *_it))
        return *_it;
    else
        return getSystem().getHardwareStorage().getConnector(*this, _id);
}

Connector const & DeviceType::getConnector(Service const & _service
                                           , integer_type _position
                                           , Direction _direction) const
{
    Connector _temp(*this, -1, _service, _position, _direction);
    std::set<Connector const *, ConnectorCompare>::const_iterator _connector = connectors_.find(&_temp);
    if (_connector != connectors_.end())
        return *(*_connector);
    std::ostringstream _errormsg;
    _errormsg << "Could not find Connector with Service " << _service.getName()
              << " and position " << _position << " for DeviceType " << name_;
    throw std::out_of_range(_errormsg.str());
}

Connector const & DeviceType::getConnector(Service const & _service
                                           , integer_type _position
                                           , Direction _direction)
{
    Connector _temp(*this, -1, _service, _position, _direction);
    std::set<Connector const *, ConnectorCompare>::const_iterator _connector = connectors_.find(&_temp);
    if (_connector != connectors_.end())
        return *(*_connector);
    return getSystem().getHardwareStorage().getConnector(*this, _service, _position, _direction);
}

Connector const & DeviceType::getConnector(std::string const & _service_name
                                           , integer_type _position
                                           , Direction _direction) const
{
    return getConnector(getSystem().getService(_service_name), _position, _direction);
}

Connector const & DeviceType::getConnector(std::string const & _service_name
                                           , integer_type _position
                                           , Direction _direction)
{
    return getConnector(getSystem().getService(_service_name), _position, _direction);
}

Parameter const & DeviceType::addParameter(integer_type _id
                                           , ParameterType const & _parametertype
                                           , integer_type _position
                                           , unsigned char _properties)
{
    std::pair<std::set<Parameter>::iterator, bool> _insert
        = id_parameters_.insert(Parameter(*this
                                          , _id
                                          , _parametertype
                                          , _position
                                          , _properties));
    Parameter const & _parameter = *(_insert.first);
    if (_insert.second)
        parameters_.insert(&_parameter);
    if (_parameter.isObservable())
        deviceobservables_.addParameter(_parameter);
    return _parameter;
}

Connector const & DeviceType::addConnector(integer_type _id
                                           , Service const & _service
                                           , integer_type _position
                                           , Direction _direction)
{
    std::pair<std::set<Connector>::iterator, bool> _insert
        = id_connectors_.insert(Connector(*this
                                          , _id
                                          , _service
                                          , _position
                                          , _direction));
    Connector const & _connector = *(_insert.first);
    if (_insert.second)
        connectors_.insert(&_connector);
    return _connector;
}

} // namespace hwd
} // namespace rpct
