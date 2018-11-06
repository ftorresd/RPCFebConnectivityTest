#include "rpct/FebConnectivityTest/FebDistributionBoard.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/Connector.h"
#include "rpct/hwd/Connection.h"
#include "rpct/hwd/DeviceObservables.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/hwd/HardwareStorage.h"

#include "rpct/FebConnectivityTest/ControlBoard.h"
#include "rpct/FebConnectivityTest/FebBoard.h"

namespace rpct {
namespace fct {

FebDistributionBoard::FebDistributionBoard(hwd::System & _system
                                           , hwd::integer_type _id
                                           , hwd::DeviceType const & _devicetype
                                           , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , controlboard_(0), position_(0)
    , febboards_(nfebboards_, 0)
{}

hwd::DeviceType & FebDistributionBoard::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<FebDistributionBoard>("FebDistributionBoard");
    hwd::DeviceType & _febdistributionboardtype = _system.registerDeviceType("FebDistributionBoard");

    hwd::Service const & _i2c
        = _system.registerService("I2C");
    _febdistributionboardtype.registerConnector(_i2c, 0, hwd::in_);

    for (std::size_t _position = 0 ; _position < nfebboards_ ; ++_position)
        _febdistributionboardtype.registerConnector(_i2c, _position, hwd::out_);

    return _febdistributionboardtype;
}

FebDistributionBoard & FebDistributionBoard::registerDevice(hwd::System & _system
                                                            , ControlBoard & _controlboard
                                                            , std::size_t _position)
{
    hwd::DeviceType const & _febdistributionboardtype = _system.getDeviceType("FebDistributionBoard");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _febdistributionboardtype);

    hwd::Device & _febdistributionboard = _system.registerDevice(_febdistributionboardtype, _properties);

    hwd::Connection _cbconnection
        = _system.getHardwareStorage().registerConnection(_controlboard
                                                          , _controlboard.getDeviceType().getConnector("I2C", _position, hwd::out_)
                                                          , _febdistributionboard
                                                          , _febdistributionboardtype.getConnector("I2C", 0, hwd::in_));

    _controlboard.addConnection(_cbconnection);
    _febdistributionboard.addConnection(_cbconnection);

    return dynamic_cast<FebDistributionBoard &>(_febdistributionboard);
}

void FebDistributionBoard::addConnection(hwd::Connection & _connection)
{
    if (_connection.getDestination() == *this)
        {
            controlboard_ = (ControlBoard *)(&(_connection.getSource()));
            position_ = _connection.getSourceConnector().getPosition();
        }
    else // Source
        febboards_.at(_connection.getSourceConnector().getPosition())
            = (FebBoard *)(&(_connection.getDestination()));
}

void FebDistributionBoard::iconfigure(hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebBoard *>::const_iterator _febboard = febboards_.begin()
             ; _febboard != febboards_.end() ; ++_febboard)
        if (*_febboard)
            (*_febboard)->configure(_mask);
}

void FebDistributionBoard::iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebBoard *>::const_iterator _febboard = febboards_.begin()
             ; _febboard != febboards_.end() ; ++_febboard)
        if (*_febboard)
            (*_febboard)->configure(_configuration, _mask);
}

void FebDistributionBoard::imonitor(hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebBoard *>::const_iterator _febboard = febboards_.begin()
             ; _febboard != febboards_.end() ; ++_febboard)
        if (*_febboard)
            (*_febboard)->monitor(_mask);
}

void FebDistributionBoard::imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebBoard *>::const_iterator _febboard = febboards_.begin()
             ; _febboard != febboards_.end() ; ++_febboard)
        if (*_febboard)
            (*_febboard)->monitor(_observables, _mask);
}

void FebDistributionBoard::iassume(hwd::Configuration const & _configuration)
{
    for (std::vector<FebBoard *>::const_iterator _febboard = febboards_.begin()
             ; _febboard != febboards_.end() ; ++_febboard)
        if (*_febboard)
            (*_febboard)->assume(_configuration);
}

} // namespace fct
} // namespace rpct
