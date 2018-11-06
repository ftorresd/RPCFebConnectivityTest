#include "rpct/FebConnectivityTest/FebChip.h"

#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/Connector.h"
#include "rpct/hwd/Connection.h"
#include "rpct/hwd/DeviceObservables.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/hwd/HardwareStorage.h"

#include "rpct/FebConnectivityTest/FebPart.h"

#ifndef OFFLINE_ANALYSIS
#include "rpct/devices/FebChip.h"
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

FebChipType::FebChipType(hwd::System & _system
                         , hwd::integer_type _id
                         , std::string const & _name)
    : hwd::DeviceType(_system, _id, _name)
    , vth_set_parameter_(0), vmon_set_parameter_(0)
    , vth_write_parameter_(0), vmon_write_parameter_(0)
    , vth_read_parameter_(0), vmon_read_parameter_(0)
{}

FebChipType & FebChipType::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<FebChip, FebChipType>("FebChip");
    hwd::DeviceType & _febchip_type = _system.registerDeviceType("FebChip");

    hwd::ParameterType const & _rpcid_type
        = _system.registerParameterType("RpcId", hwd::DataType::integer_);
    _febchip_type.registerParameter(_rpcid_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _vth_set_type
        = _system.registerParameterType("VThSet", hwd::DataType::integer_);
    _febchip_type.registerParameter(_vth_set_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _vmon_set_type
        = _system.registerParameterType("VMonSet", hwd::DataType::integer_);
    _febchip_type.registerParameter(_vmon_set_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _auto_correct_type
        = _system.registerParameterType("AutoCorrect", hwd::DataType::integer_);
    _febchip_type.registerParameter(_auto_correct_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _vth_write_type
        = _system.registerParameterType("VThWrite", hwd::DataType::integer_);
    _febchip_type.registerParameter(_vth_write_type, 0, hwd::Parameter::is_observable_);

    hwd::ParameterType const & _vmon_write_type
        = _system.registerParameterType("VMonWrite", hwd::DataType::integer_);
    _febchip_type.registerParameter(_vmon_write_type, 0, hwd::Parameter::is_observable_);

    hwd::ParameterType const & _vth_read_type
        = _system.registerParameterType("VThRead", hwd::DataType::integer_);
    _febchip_type.registerParameter(_vth_read_type, 0, hwd::Parameter::is_observable_);

    hwd::ParameterType const & _vmon_read_type
        = _system.registerParameterType("VMonRead", hwd::DataType::integer_);
    _febchip_type.registerParameter(_vmon_read_type, 0, hwd::Parameter::is_observable_);

    hwd::Service const & _host
        = _system.registerService("Host");
    _febchip_type.registerConnector(_host, 0, hwd::in_);

    return dynamic_cast<FebChipType &>(_febchip_type);
}

hwd::Parameter const & FebChipType::addParameter(hwd::integer_type _id
                                                 , hwd::ParameterType const & _parametertype
                                                 , hwd::integer_type _position
                                                 , unsigned char _properties)
{
    hwd::Parameter const & _parameter
        = hwd::DeviceType::addParameter(_id, _parametertype, _position, _properties);
    if (_parametertype.getName() == "VThSet")
        vth_set_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "VMonSet")
        vmon_set_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "AutoCorrect")
        auto_correct_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "VThWrite")
        vth_write_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "VMonWrite")
        vmon_write_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "VThRead")
        vth_read_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "VMonRead")
        vmon_read_parameter_ = &(_parameter);
    return _parameter;
}

FebChip::FebChip(hwd::System & _system
                 , hwd::integer_type _id
                 , hwd::DeviceType const & _devicetype
                 , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , rpcid_(0)
    , febpart_(0), position_(0)
    , febchip_type_(dynamic_cast<FebChipType const &>(_devicetype))
    , vth_set_(0), vmon_set_(0), auto_correct_(0)
    , vth_write_(0), vmon_write_(0)
    , vth_read_(0), vmon_read_(0)
{
    hwd::Parameter const & _rpcid = getDeviceType().getParameter("RpcId", 0);
    if (_properties.hasParameterSetting(_rpcid))
        _properties.getParameterSetting(_rpcid, rpcid_);
}

FebChipType & FebChip::registerDeviceType(hwd::System & _system)
{
    return FebChipType::registerDeviceType(_system);
}

FebChip & FebChip::registerDevice(hwd::System & _system
                                  , int _rpcid
                                  , hwd::DeviceFlags const & _deviceflags
                                  , FebPart & _febpart
                                  , std::size_t _position)
{
    hwd::DeviceType const & _febchip_type = _system.getDeviceType("FebChip");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _febchip_type);

    _properties.registerParameterSetting(_febchip_type.getParameter("RpcId", 0), _rpcid);
    _properties.registerDeviceFlags(_deviceflags);
    hwd::Device & _febchip = _system.registerDevice(_febchip_type, _properties);

    hwd::Connection _fpconnection
        = _system.getHardwareStorage().registerConnection(_febpart
                                                          , _febpart.getDeviceType().getConnector("Host", _position, hwd::out_)
                                                          , _febchip
                                                          , _febchip_type.getConnector("Host", 0, hwd::in_));

    _febpart.addConnection(_fpconnection);
    _febchip.addConnection(_fpconnection);

    return dynamic_cast<FebChip &>(_febchip);
}

void FebChip::addConnection(hwd::Connection & _connection)
{
    febpart_ = (FebPart *)(&(_connection.getSource()));
    position_ = _connection.getSourceConnector().getPosition();
}

void FebChip::iconfigure(hwd::DeviceConfiguration const & _deviceconfiguration, hwd::DeviceFlagsMask const & _mask)
{
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVThSetParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVThSetParameter(), vth_set_);
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVMonSetParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVMonSetParameter(), vmon_set_);
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getAutoCorrectParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getAutoCorrectParameter(), auto_correct_);
#ifndef OFFLINE_ANALYSIS
    getDeviceState().setState(getRpctFebChip().getState());
    getDeviceFlags().setConfigured(getRpctFebChip().isConfigured());
    getDeviceFlags().setBroken(getRpctFebChip().isBroken());
    getDeviceFlags().setWarm(getRpctFebChip().isWarm());
    getDeviceFlags().setDisabled(getRpctFebChip().isDisabled());
#endif // OFFLINE_ANALYSIS
}

void FebChip::imonitor(hwd::DeviceObservables const & _deviceobservables, hwd::DeviceFlagsMask const & _mask)
{
#ifndef OFFLINE_ANALYSIS
    if (_deviceobservables.hasParameter(febchip_type_.getVThWriteParameter()))
        {
            vth_write_ = getRpctFebChip().getVTHSet();
            getSystem().publish(*this, febchip_type_.getVThWriteParameter(), vth_write_);
        }
    if (_deviceobservables.hasParameter(febchip_type_.getVMonWriteParameter()))
        {
            vmon_write_ = getRpctFebChip().getVMONSet();
            getSystem().publish(*this, febchip_type_.getVMonWriteParameter(), vmon_write_);
        }

    if (_deviceobservables.hasParameter(febchip_type_.getVThReadParameter()))
        {
            vth_read_ = getRpctFebChip().getVTH();
            getSystem().publish(*this, febchip_type_.getVThReadParameter(), vth_read_);
        }
    if (_deviceobservables.hasParameter(febchip_type_.getVMonReadParameter()))
        {
            vmon_read_ = getRpctFebChip().getVMON();
            getSystem().publish(*this, febchip_type_.getVMonReadParameter(), vmon_read_);
        }
    getDeviceState().setState(getRpctFebChip().getState());
    getDeviceFlags().setConfigured(getRpctFebChip().isConfigured());
    getDeviceFlags().setBroken(getRpctFebChip().isBroken());
    getDeviceFlags().setWarm(getRpctFebChip().isWarm());
    getDeviceFlags().setDisabled(getRpctFebChip().isDisabled());
#endif // OFFLINE_ANALYSIS
}

void FebChip::iassume(hwd::DeviceConfiguration const & _deviceconfiguration)
{
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVThSetParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVThSetParameter(), vth_set_);
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVMonSetParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVMonSetParameter(), vmon_set_);
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getAutoCorrectParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getAutoCorrectParameter(), auto_correct_);

    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVThWriteParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVThWriteParameter(), vth_write_);
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVMonWriteParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVMonWriteParameter(), vmon_write_);
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVThReadParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVThReadParameter(), vth_read_);
    if (_deviceconfiguration.hasParameterSetting(febchip_type_.getVMonReadParameter()))
        _deviceconfiguration.getParameterSetting(febchip_type_.getVMonReadParameter(), vmon_read_);

#ifndef OFFLINE_ANALYSIS
    getDeviceState().setState(getRpctFebChip().getState());
    getDeviceFlags().setConfigured(getRpctFebChip().isConfigured());
    getDeviceFlags().setBroken(getRpctFebChip().isBroken());
    getDeviceFlags().setWarm(getRpctFebChip().isWarm());
    getDeviceFlags().setDisabled(getRpctFebChip().isDisabled());
#endif // OFFLINE_ANALYSIS
}

} // namespace fct
} // namespace rpct
