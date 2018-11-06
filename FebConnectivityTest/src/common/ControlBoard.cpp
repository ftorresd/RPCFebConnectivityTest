#include "rpct/FebConnectivityTest/ControlBoard.h"

#include "rpct/hwd/Service.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Connector.h"
#include "rpct/hwd/Connection.h"
#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/DeviceObservables.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/hwd/HardwareStorage.h"

#include "rpct/FebConnectivityTest/LinkBox.h"
#include "rpct/FebConnectivityTest/FebDistributionBoard.h"

namespace rpct {
namespace fct {

ControlBoard::ControlBoard(hwd::System & _system
                           , hwd::integer_type _id
                           , hwd::DeviceType const & _devicetype
                           , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , rpcid_(0), name_("")
    , linkbox_(0), position_(0)
    , febdistributionboards_(nfebdistributionboards_, 0)
{
    hwd::Parameter const & _rpcid = getDeviceType().getParameter("RpcId", 0);
    if (_properties.hasParameterSetting(_rpcid))
        _properties.getParameterSetting(_rpcid, rpcid_);
    hwd::Parameter const & _name = getDeviceType().getParameter("Name", 0);
    if (_properties.hasParameterSetting(_name))
        _properties.getParameterSetting(_name, name_);
}

hwd::DeviceType & ControlBoard::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<ControlBoard>("ControlBoard");
    hwd::DeviceType & _controlboardtype = _system.registerDeviceType("ControlBoard");

    hwd::ParameterType const & _rpcid_type
        = _system.registerParameterType("RpcId", hwd::DataType::integer_);
    _controlboardtype.registerParameter(_rpcid_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _name_type
        = _system.registerParameterType("Name", hwd::DataType::text_);
    _controlboardtype.registerParameter(_name_type, 0, hwd::Parameter::is_property_);

    hwd::Service const & _crate
        = _system.registerService("Crate");
    _controlboardtype.registerConnector(_crate, 0, hwd::in_);

    hwd::Service const & _i2c
        = _system.registerService("I2C");
    for (std::size_t _position = 0 ; _position < nfebdistributionboards_ ; ++_position)
        _controlboardtype.registerConnector(_i2c, _position + 1, hwd::out_);

    return _controlboardtype;
}

ControlBoard & ControlBoard::registerDevice(hwd::System & _system
                                            , int _rpcid
                                            , hwd::DeviceFlags const & _deviceflags
                                            , std::string const & _name
                                            , LinkBox & _linkbox
                                            , std::size_t _position)
{
    hwd::DeviceType const & _controlboardtype = _system.getDeviceType("ControlBoard");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _controlboardtype);

    _properties.registerParameterSetting(_controlboardtype.getParameter("RpcId", 0), _rpcid);
    _properties.registerParameterSetting(_controlboardtype.getParameter("Name", 0), _name);
    _properties.registerDeviceFlags(_deviceflags);
    hwd::Device & _controlboard = _system.registerDevice(_controlboardtype, _properties);

    hwd::Connection _lbconnection
        = _system.getHardwareStorage().registerConnection(_linkbox
                                                          , _linkbox.getDeviceType().getConnector("Crate", _position, hwd::out_)
                                                          , _controlboard
                                                          , _controlboardtype.getConnector("Crate", 0, hwd::in_));

    _linkbox.addConnection(_lbconnection);
    _controlboard.addConnection(_lbconnection);

    return dynamic_cast<ControlBoard &>(_controlboard);
}

void ControlBoard::addConnection(hwd::Connection & _connection)
{
    if (_connection.getSourceConnector().getService().getName() == "Crate")
        {
            linkbox_ = (LinkBox *)(&(_connection.getSource()));
            position_ = _connection.getSourceConnector().getPosition();
        }
    else // I2C
        febdistributionboards_.at(_connection.getSourceConnector().getPosition() - 1)
            = (FebDistributionBoard *)(&(_connection.getDestination()));
}

void ControlBoard::iconfigure(hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebDistributionBoard *>::const_iterator _febdistributionboard = febdistributionboards_.begin()
             ; _febdistributionboard != febdistributionboards_.end() ; ++_febdistributionboard)
        if (*_febdistributionboard)
            (*_febdistributionboard)->configure(_mask);
}

void ControlBoard::iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebDistributionBoard *>::const_iterator _febdistributionboard = febdistributionboards_.begin()
             ; _febdistributionboard != febdistributionboards_.end() ; ++_febdistributionboard)
        if (*_febdistributionboard)
            (*_febdistributionboard)->configure(_configuration, _mask);
}

void ControlBoard::imonitor(hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebDistributionBoard *>::const_iterator _febdistributionboard = febdistributionboards_.begin()
             ; _febdistributionboard != febdistributionboards_.end() ; ++_febdistributionboard)
        if (*_febdistributionboard)
            (*_febdistributionboard)->monitor(_mask);
}

void ControlBoard::imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebDistributionBoard *>::const_iterator _febdistributionboard = febdistributionboards_.begin()
             ; _febdistributionboard != febdistributionboards_.end() ; ++_febdistributionboard)
        if (*_febdistributionboard)
            (*_febdistributionboard)->monitor(_observables, _mask);
}

void ControlBoard::iassume(hwd::Configuration const & _configuration)
{
    for (std::vector<FebDistributionBoard *>::const_iterator _febdistributionboard = febdistributionboards_.begin()
             ; _febdistributionboard != febdistributionboards_.end() ; ++_febdistributionboard)
        if (*_febdistributionboard)
            (*_febdistributionboard)->assume(_configuration);
}

} // namespace fct
} // namespace rpct
