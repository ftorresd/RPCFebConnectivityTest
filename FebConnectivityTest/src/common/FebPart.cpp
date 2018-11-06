#include "rpct/FebConnectivityTest/FebPart.h"

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

#include "rpct/FebConnectivityTest/FebBoard.h"
#include "rpct/FebConnectivityTest/FebChip.h"
#include "rpct/FebConnectivityTest/FebConnector.h"

#ifndef OFFLINE_ANALYSIS
#include "rpct/devices/FebPart.h"
#include "rpct/ii/feb_const.h"
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

FebPartType::FebPartType(hwd::System & _system
                         , hwd::integer_type _id
                         , std::string const & _name)
    : hwd::DeviceType(_system, _id, _name)
    , temperature_parameter_(0)
{}

FebPartType & FebPartType::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<FebPart, FebPartType>("FebPart");
    hwd::DeviceType & _febpart_type
        = _system.registerDeviceType("FebPart");

    hwd::ParameterType const & _temperature_type
        =  _system.registerParameterType("Temperature", hwd::DataType::float_);
    _febpart_type.registerParameter(_temperature_type, 0, hwd::Parameter::is_observable_);

    hwd::Service const & _host
        = _system.registerService("Host");
    _febpart_type.registerConnector(_host, 0, hwd::in_);

    for (std::size_t _position = 0 ; _position < FebPart::nfebchips_ ; ++_position)
        _febpart_type.registerConnector(_host, _position, hwd::out_);

    hwd::Service const & _signal
        = _system.registerService("Signal");
    _febpart_type.registerConnector(_signal, 0, hwd::out_);

    return dynamic_cast<FebPartType &>(_febpart_type);
}

hwd::Parameter const & FebPartType::addParameter(hwd::integer_type _id
                                                 , hwd::ParameterType const & _parametertype
                                                 , hwd::integer_type _position
                                                 , unsigned char _properties)
{
    hwd::Parameter const & _parameter
        = hwd::DeviceType::addParameter(_id, _parametertype, _position, _properties);
    if (_parametertype.getName() == "Temperature")
        temperature_parameter_ = &(_parameter);
    return _parameter;
}

FebPart::FebPart(hwd::System & _system
                 , hwd::integer_type _id
                 , hwd::DeviceType const & _devicetype
                 , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , febboard_(0), position_(0)
    , febchips_(nfebchips_, 0)
    , febpart_type_(dynamic_cast<FebPartType const &>(_devicetype))
    , temperature_(0.)
{}

FebPartType & FebPart::registerDeviceType(hwd::System & _system)
{
    return FebPartType::registerDeviceType(_system);
}

FebPart & FebPart::registerDevice(hwd::System & _system
                                  , FebBoard & _febboard
                                  , std::size_t _position)
{
    hwd::DeviceType const & _febpart_type = _system.getDeviceType("FebPart");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _febpart_type);

    hwd::Device & _febpart = _system.registerDevice(_febpart_type, _properties);

    hwd::Connection _fbconnection
        = _system.getHardwareStorage().registerConnection(_febboard
                                                          , _febboard.getDeviceType().getConnector("Host", _position, hwd::out_)
                                                          , _febpart
                                                          , _febpart_type.getConnector("Host", 0, hwd::in_));

    _febboard.addConnection(_fbconnection);
    _febpart.addConnection(_fbconnection);

    return dynamic_cast<FebPart &>(_febpart);
}

void FebPart::addConnection(hwd::Connection & _connection)
{
    if (_connection.getDestination() == *this)
        {
            febboard_ = (FebBoard *)(&(_connection.getSource()));
            position_ = _connection.getSourceConnector().getPosition();
        }
    else // Source
        {
            if (_connection.getSourceConnector().getService().getName() == "Host")
                febchips_.at(_connection.getSourceConnector().getPosition())
                    = (FebChip *)(&(_connection.getDestination()));
            else // Signal
                febconnector_ = (FebConnector *)(&(_connection.getDestination()));
        }
}

void FebPart::iconfigure(hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebChip *>::const_iterator _febchip = febchips_.begin()
             ; _febchip != febchips_.end() ; ++_febchip)
        if (*_febchip)
            (*_febchip)->configure(_mask);
#ifndef OFFLINE_ANALYSIS
    getDeviceState().setState(getRpctFebPart().getState());
    getDeviceFlags().setConfigured(getRpctFebPart().isConfigured());
    getDeviceFlags().setBroken(getRpctFebPart().isBroken());
    getDeviceFlags().setWarm(getRpctFebPart().isWarm());
    getDeviceFlags().setDisabled(getRpctFebPart().isDisabled());
#endif // OFFLINE_ANALYSIS
}

void FebPart::iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebChip *>::const_iterator _febchip = febchips_.begin()
             ; _febchip != febchips_.end() ; ++_febchip)
        if (*_febchip)
            (*_febchip)->configure(_configuration, _mask);
#ifndef OFFLINE_ANALYSIS
    getDeviceState().setState(getRpctFebPart().getState());
    getDeviceFlags().setConfigured(getRpctFebPart().isConfigured());
    getDeviceFlags().setBroken(getRpctFebPart().isBroken());
    getDeviceFlags().setWarm(getRpctFebPart().isWarm());
    getDeviceFlags().setDisabled(getRpctFebPart().isDisabled());
#endif // OFFLINE_ANALYSIS
}

void FebPart::imonitor(hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebChip *>::const_iterator _febchip = febchips_.begin()
             ; _febchip != febchips_.end() ; ++_febchip)
        if (*_febchip)
            (*_febchip)->monitor(_mask);
#ifndef OFFLINE_ANALYSIS
    getDeviceState().setState(getRpctFebPart().getState());
    getDeviceFlags().setConfigured(getRpctFebPart().isConfigured());
    getDeviceFlags().setBroken(getRpctFebPart().isBroken());
    getDeviceFlags().setWarm(getRpctFebPart().isWarm());
    getDeviceFlags().setDisabled(getRpctFebPart().isDisabled());
#endif // OFFLINE_ANALYSIS
}

void FebPart::imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask)
{
    for (std::vector<FebChip *>::const_iterator _febchip = febchips_.begin()
             ; _febchip != febchips_.end() ; ++_febchip)
        if (*_febchip)
            (*_febchip)->monitor(_observables, _mask);
#ifndef OFFLINE_ANALYSIS
    getDeviceState().setState(getRpctFebPart().getState());
    getDeviceFlags().setConfigured(getRpctFebPart().isConfigured());
    getDeviceFlags().setBroken(getRpctFebPart().isBroken());
    getDeviceFlags().setWarm(getRpctFebPart().isWarm());
    getDeviceFlags().setDisabled(getRpctFebPart().isDisabled());
#endif // OFFLINE_ANALYSIS
}

void FebPart::imonitor(hwd::DeviceObservables const & _deviceobservables, hwd::DeviceFlagsMask const & _mask)
{
#ifndef OFFLINE_ANALYSIS
    if (_deviceobservables.hasParameter(febpart_type_.getTemperatureParameter()))
        {
            temperature_ = getRpctFebPart().getTemperature();
            getSystem().publish(*this, febpart_type_.getTemperatureParameter(), temperature_);
        }
#endif // OFFLINE_ANALYSIS
}

void FebPart::iassume(hwd::Configuration const & _configuration)
{
    for (std::vector<FebChip *>::const_iterator _febchip = febchips_.begin()
             ; _febchip != febchips_.end() ; ++_febchip)
        if (*_febchip)
            (*_febchip)->assume(_configuration);
}

void FebPart::iassume(hwd::DeviceConfiguration const & _deviceconfiguration)
{
    if (_deviceconfiguration.hasParameterSetting(febpart_type_.getTemperatureParameter()))
        _deviceconfiguration.getParameterSetting(febpart_type_.getTemperatureParameter(), temperature_);
}

} // namespace fct
} // namespace rpct
