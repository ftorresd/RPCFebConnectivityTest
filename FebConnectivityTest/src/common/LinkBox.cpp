#include "rpct/FebConnectivityTest/LinkBox.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/Connector.h"
#include "rpct/hwd/Connection.h"
#include "rpct/hwd/DeviceObservables.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"
#include "rpct/hwd/System.h"
#include "rpct/hwd/DeviceFlagsMask.h"

#include "rpct/hwd/HardwareStorage.h"

#include "rpct/FebConnectivityTest/LinkBoard.h"
#include "rpct/FebConnectivityTest/ControlBoard.h"

namespace rpct {
namespace fct {

LinkBox::LinkBox(hwd::System & _system
                 , hwd::integer_type _id
                 , hwd::DeviceType const & _devicetype
                 , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , linkboards_(nlinkboards_, 0)
    , controlboards_(ncontrolboards_, 0)
    , active_parameter_(_devicetype.getParameter("Active", 0))
    , active_(false)
{
    hwd::Parameter const & _rpcid = getDeviceType().getParameter("RpcId", 0);
    if (_properties.hasParameterSetting(_rpcid))
        _properties.getParameterSetting(_rpcid, rpcid_);
    hwd::Parameter const & _name = getDeviceType().getParameter("Name", 0);
    if (_properties.hasParameterSetting(_name))
        _properties.getParameterSetting(_name, name_);
    hwd::Parameter const & _location = getDeviceType().getParameter("Location", 0);
    if (_properties.hasParameterSetting(_location))
        _properties.getParameterSetting(_location, location_);
}

hwd::DeviceType & LinkBox::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<LinkBox>("LinkBox");
    hwd::DeviceType & _linkboxtype = _system.registerDeviceType("LinkBox");

    hwd::ParameterType const & _rpcid_type
        =  _system.registerParameterType("RpcId", hwd::DataType::integer_);
    _linkboxtype.registerParameter(_rpcid_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _name_type
        =  _system.registerParameterType("Name", hwd::DataType::text_);
    _linkboxtype.registerParameter(_name_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _location_type
        =  _system.registerParameterType("Location", hwd::DataType::text_);
    _linkboxtype.registerParameter(_location_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _active_type
        =  _system.registerParameterType("Active", hwd::DataType::integer_);
    _linkboxtype.registerParameter(_active_type, 0, hwd::Parameter::is_observable_);

    hwd::Service const & _crate
        = _system.registerService("Crate");
    for (std::size_t _position = 0 ; _position < nboards_ ; ++_position)
        _linkboxtype.registerConnector(_crate, _position + 1, hwd::out_);

    return _linkboxtype;
}

LinkBox & LinkBox::registerDevice(hwd::System & _system
                                  , int _rpcid
                                  , hwd::DeviceFlags const & _deviceflags
                                  , std::string const & _name
                                  , std::string const & _location)
{
    hwd::DeviceType const & _linkboxtype = _system.getDeviceType("LinkBox");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _linkboxtype);

    _properties.registerParameterSetting(_linkboxtype.getParameter("RpcId", 0), _rpcid);
    _properties.registerParameterSetting(_linkboxtype.getParameter("Name", 0), _name);
    _properties.registerParameterSetting(_linkboxtype.getParameter("Location", 0), _location);
    _properties.registerDeviceFlags(_deviceflags);
    return dynamic_cast<LinkBox &>(_system.registerDevice(_linkboxtype, _properties));
}

void LinkBox::addConnection(hwd::Connection & _connection)
{
    if (_connection.getSourceConnector().getService().getName() == "Crate")
        {
            Device & _board = _connection.getDestination();
            int _position = _connection.getSourceConnector().getPosition();
            if (_board.getDeviceType().getName() == "LinkBoard")
                {
                    _position = _position - (_position / 10) - 1;
                    linkboards_.at(_position) = (LinkBoard *)(&_board);
                }
            else // ControlBoard
                {
                    _position = (_position / 10) - 1;
                    controlboards_.at(_position) = (ControlBoard *)(&_board);
                }
        }
}

void LinkBox::iconfigure(hwd::DeviceFlagsMask const & _mask)
{
    if (controlboards_.at(0))
        controlboards_.at(0)->configure(_mask);
    if (controlboards_.at(1))
        controlboards_.at(1)->configure(_mask);
    for (std::vector<LinkBoard *>::const_iterator _linkboard = linkboards_.begin()
             ; _linkboard != linkboards_.end() ; ++_linkboard)
        if (*_linkboard)
            (*_linkboard)->configure(_mask);
}

void LinkBox::iconfigure(hwd::Configuration const & _configuration
                         , hwd::DeviceFlagsMask const & _mask)
{
    if (controlboards_.at(0))
        controlboards_.at(0)->configure(_configuration, _mask);
    if (controlboards_.at(1))
        controlboards_.at(1)->configure(_configuration, _mask);
    for (std::vector<LinkBoard *>::const_iterator _linkboard = linkboards_.begin()
             ; _linkboard != linkboards_.end() ; ++_linkboard)
        if (*_linkboard)
            (*_linkboard)->configure(_configuration, _mask);
}

void LinkBox::imonitor(hwd::DeviceFlagsMask const & _mask)
{
    if (controlboards_.at(0))
        controlboards_.at(0)->monitor(_mask);
    if (controlboards_.at(1))
        controlboards_.at(1)->monitor(_mask);
    for (std::vector<LinkBoard *>::const_iterator _linkboard = linkboards_.begin()
             ; _linkboard != linkboards_.end() ; ++_linkboard)
        if (*_linkboard)
            (*_linkboard)->monitor(_mask);
}

void LinkBox::imonitor(hwd::Observables const & _observables
                       , hwd::DeviceFlagsMask const & _mask)
{
    if (controlboards_.at(0))
        controlboards_.at(0)->monitor(_observables, _mask);
    if (controlboards_.at(1))
        controlboards_.at(1)->monitor(_observables, _mask);
    for (std::vector<LinkBoard *>::const_iterator _linkboard = linkboards_.begin()
             ; _linkboard != linkboards_.end() ; ++_linkboard)
        if (*_linkboard)
            (*_linkboard)->monitor(_observables, _mask);
}

void LinkBox::iassume(hwd::Configuration const & _configuration)
{
    if (controlboards_.at(0))
        controlboards_.at(0)->assume(_configuration);
    if (controlboards_.at(1))
        controlboards_.at(1)->assume(_configuration);
    for (std::vector<LinkBoard *>::const_iterator _linkboard = linkboards_.begin()
             ; _linkboard != linkboards_.end() ; ++_linkboard)
        if (*_linkboard)
            (*_linkboard)->assume(_configuration);
}

void LinkBox::iassume(hwd::DeviceConfiguration const & _deviceconfiguration)
{
    if (_deviceconfiguration.hasParameterSetting(active_parameter_))
        _deviceconfiguration.getParameterSetting(active_parameter_, active_);
}

void LinkBox::setActive(bool _active)
{
    if (active_ != _active)
        {
            active_ = _active;
            getSystem().publish(*this, active_parameter_, active_);
        }
}

} // namespace fct
} // namespace rpct
