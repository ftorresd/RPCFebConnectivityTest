#include "rpct/FebConnectivityTest/FebBoard.h"

#include "log4cplus/loggingmacros.h"

#include "rpct/hwd/DeviceFlagsMask.h"
#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/Connector.h"
#include "rpct/hwd/Connection.h"
#include "rpct/hwd/Observables.h"
#include "rpct/hwd/DeviceObservables.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/hwd/HardwareStorage.h"

#include "rpct/FebConnectivityTest/FebDistributionBoard.h"
#include "rpct/FebConnectivityTest/FebPart.h"
#include "rpct/FebConnectivityTest/FebChip.h"

#ifndef OFFLINE_ANALYSIS
#include "rpct/devices/FebBoard.h"
#include "rpct/devices/ConfigurationSetImpl.h"
#include "rpct/xdaqutils/FebChipConfiguration.h"
#include "rpct/ii/Monitorable.h"
#include "rpct/ii/feb_const.h"
#include "rpct/ii/ConfigurationFlags.h"
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

std::size_t const FebBoard::nfebparts_[2] = {1, 2};

log4cplus::Logger FebBoard::logger_ = log4cplus::Logger::getInstance("FebConnectivityFebBoard");

FebBoard::FebBoard(hwd::System & _system
                   , hwd::integer_type _id
                   , hwd::DeviceType const & _devicetype
                   , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , rpcid_(0), name_(""), feb_type_(0)
    , febdistributionboard_(0), position_(0)
    , febparts_(1, (FebPart *)(0))
#ifndef OFFLINE_ANALYSIS
    , rpct_febboard_(0)
    , febpart_type_(dynamic_cast<FebPartType const &>(_system.getDeviceType("FebPart")))
    , febchip_type_(dynamic_cast<FebChipType const &>(_system.getDeviceType("FebChip")))
#endif // OFFLINE_ANALYSIS
{
    hwd::Parameter const & _rpcid = getDeviceType().getParameter("RpcId", 0);
    if (_properties.hasParameterSetting(_rpcid))
        _properties.getParameterSetting(_rpcid, rpcid_);
    hwd::Parameter const & _name = getDeviceType().getParameter("Name", 0);
    if (_properties.hasParameterSetting(_name))
        _properties.getParameterSetting(_name, name_);
    hwd::Parameter const & _feb_type = getDeviceType().getParameter("FebType", 0);
    if (_properties.hasParameterSetting(_feb_type))
        _properties.getParameterSetting(_feb_type, feb_type_);
    febparts_.assign(nfebparts_[feb_type_], 0);
}

hwd::DeviceType & FebBoard::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<FebBoard>("FebBoard");
    hwd::DeviceType & _febboardtype = _system.registerDeviceType("FebBoard");

    hwd::ParameterType const & _rpcid_type
        =  _system.registerParameterType("RpcId", hwd::DataType::integer_);
    _febboardtype.registerParameter(_rpcid_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _name_type
        =  _system.registerParameterType("Name", hwd::DataType::text_);
    _febboardtype.registerParameter(_name_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _feb_type_type
        =  _system.registerParameterType("FebType", hwd::DataType::integer_);
    _febboardtype.registerParameter(_feb_type_type, 0, hwd::Parameter::is_property_);

    hwd::Service const & _i2c
        = _system.registerService("I2C");
    _febboardtype.registerConnector(_i2c, 0, hwd::in_);

    hwd::Service const & _host
        = _system.registerService("Host");
    for (std::size_t _position = 0 ; _position < max_nfebparts_ ; ++_position)
        _febboardtype.registerConnector(_host, _position, hwd::out_);

    return _febboardtype;
}

FebBoard & FebBoard::registerDevice(hwd::System & _system
                                    , int _rpcid
                                    , hwd::DeviceFlags const & _deviceflags
                                    , std::string const & _name
                                    , int _feb_type
                                    , FebDistributionBoard & _febdistributionboard
                                    , std::size_t _position)
{
    hwd::DeviceType const & _febboardtype = _system.getDeviceType("FebBoard");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _febboardtype);

    _properties.registerParameterSetting(_febboardtype.getParameter("RpcId", 0), _rpcid);
    _properties.registerParameterSetting(_febboardtype.getParameter("Name", 0), _name);
    _properties.registerParameterSetting(_febboardtype.getParameter("FebType", 0), _feb_type);
    _properties.registerDeviceFlags(_deviceflags);
    hwd::Device & _febboard = _system.registerDevice(_febboardtype, _properties);

    hwd::Connection _fdbconnection
        = _system.getHardwareStorage().registerConnection(_febdistributionboard
                                                          , _febdistributionboard.getDeviceType().getConnector("I2C", _position, hwd::out_)
                                                          , _febboard
                                                          , _febboardtype.getConnector("I2C", 0, hwd::in_));

    _febdistributionboard.addConnection(_fdbconnection);
    _febboard.addConnection(_fdbconnection);

    return dynamic_cast<FebBoard &>(_febboard);
}

void FebBoard::addConnection(hwd::Connection & _connection)
{
    if (_connection.getDestination() == *this)
        {
            febdistributionboard_ = (FebDistributionBoard *)(&(_connection.getSource()));
            position_ = _connection.getSourceConnector().getPosition();
        }
    else // Source
        febparts_.at(_connection.getSourceConnector().getPosition())
            = (FebPart *)(&(_connection.getDestination()));
}

void FebBoard::iconfigure(hwd::DeviceFlagsMask const & _mask)
{
#ifndef OFFLINE_ANALYSIS
    try {
        getRpctFebBoard().configure(_mask.includesDisabled() ? ConfigurationFlags::INCLUDE_DISABLED : 0x0);
    } catch(std::exception & _e) {
        LOG4CPLUS_WARN(logger_, "Exception trying to configure FebBoard " << name_ << ": " << _e.what());
    }
    getDeviceState().setState(getRpctFebBoard().getState());
    getDeviceFlags().setConfigured(getRpctFebBoard().isConfigured());
    getDeviceFlags().setBroken(getRpctFebBoard().isBroken());
    getDeviceFlags().setWarm(getRpctFebBoard().isWarm());
    getDeviceFlags().setDisabled(getRpctFebBoard().isDisabled());
#endif // OFFLINE_ANALYSIS
    for (std::vector<FebPart *>::const_iterator _febpart = febparts_.begin()
             ; _febpart != febparts_.end() ; ++_febpart)
        if (*_febpart)
            (*_febpart)->configure(_mask);
}

void FebBoard::iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask)
{
#ifndef OFFLINE_ANALYSIS
    std::map<int, rpct::DeviceSettingsPtr> _rpct_settings, _rpct_correct_settings;
    rpct::ConfigurationSetImpl _rpct_configuration, _rpct_correct_configuration;
    bool _any_auto_correct(false);
    for (std::vector<FebPart *>::const_iterator _febpart = febparts_.begin()
             ; _febpart != febparts_.end() ; ++_febpart)
        if (*_febpart && _mask.matches((*_febpart)->getDeviceFlags())) {
            std::vector<FebChip *> const & _febchips = (*_febpart)->getFebChips();
            std::vector<FebChip *>::const_iterator _febchip_end = _febchips.end();
            for (std::vector<FebChip *>::const_iterator _febchip = _febchips.begin()
                     ; _febchip != _febchip_end ; ++_febchip)
                if (*_febchip && _mask.matches((*_febchip)->getDeviceFlags())
                    && _configuration.hasDeviceConfiguration(*(*_febchip))) {
                    hwd::DeviceConfiguration const & _hwd_deviceconfiguration
                        = _configuration.getDeviceConfiguration(*(*_febchip));
                    unsigned int _vth((*_febchip)->getVThSet()), _vmon((*_febchip)->getVMonSet());
                    bool _auto_correct((*_febchip)->getAutoCorrect());
                    if (_hwd_deviceconfiguration.hasParameterSetting(febchip_type_.getVThSetParameter()))
                        _hwd_deviceconfiguration.getParameterSetting(febchip_type_.getVThSetParameter(), _vth);
                    if (_hwd_deviceconfiguration.hasParameterSetting(febchip_type_.getVMonSetParameter()))
                        _hwd_deviceconfiguration.getParameterSetting(febchip_type_.getVMonSetParameter(), _vmon);
                    if (_hwd_deviceconfiguration.hasParameterSetting(febchip_type_.getAutoCorrectParameter()))
                        _hwd_deviceconfiguration.getParameterSetting(febchip_type_.getAutoCorrectParameter(), _auto_correct);
                    if (_rpct_settings.find(_hwd_deviceconfiguration.getId()) == _rpct_settings.end())
                        { // to avoid creation and immediate destruction
                            _rpct_settings.insert(std::pair<int
                                                  , rpct::DeviceSettingsPtr>(_hwd_deviceconfiguration.getId()
                                                                             , rpct::DeviceSettingsPtr(new rpct::xdaqutils::FebChipConfiguration(_hwd_deviceconfiguration.getId()
                                                                                                                                                 , _vth, _vmon
                                                                                                                                                 , 0, 0))));
                            if (_auto_correct)
                                _rpct_correct_settings.insert(std::pair<int
                                                              , rpct::DeviceSettingsPtr>(_hwd_deviceconfiguration.getId()
                                                                                         , rpct::DeviceSettingsPtr(new rpct::xdaqutils::FebChipConfiguration(_hwd_deviceconfiguration.getId()
                                                                                                                                                             , _vth, _vmon
                                                                                                                                                             , rpct::preset::feb::offset_keep_, rpct::preset::feb::offset_keep_))));
                        }
                    _rpct_configuration.addDeviceSettings((*_febchip)->getRPCId()
                                                          , _rpct_settings[_hwd_deviceconfiguration.getId()]);
                    if (_auto_correct)
                        {
                            _rpct_correct_configuration.addDeviceSettings((*_febchip)->getRPCId()
                                                                          , _rpct_correct_settings[_hwd_deviceconfiguration.getId()]);
                            _any_auto_correct = true;
                        }
                }
        }
    try {
        getRpctFebBoard().configure(&_rpct_configuration, _mask.includesDisabled() ? ConfigurationFlags::INCLUDE_DISABLED : 0x0);
        if (_any_auto_correct)
            {
                rpct::IMonitorable::MonitorItems _rpct_monitoritems;
                _rpct_monitoritems.push_back(rpct::monitoritem::feb::vth_);
                _rpct_monitoritems.push_back(rpct::monitoritem::feb::vmon_);
                _rpct_monitoritems.push_back(rpct::monitoritem::feb::pcf8574a_);
                getRpctFebBoard().monitor(_rpct_monitoritems, _mask.includesDisabled() ? ConfigurationFlags::INCLUDE_DISABLED : 0x0);
                getRpctFebBoard().configure(&_rpct_correct_configuration, _mask.includesDisabled() ? ConfigurationFlags::INCLUDE_DISABLED : 0x0);
            }
    } catch(std::exception & _e) {
        LOG4CPLUS_WARN(logger_, "Exception trying to configure FebBoard " << name_ << ": " << _e.what());
    }
    getDeviceState().setState(getRpctFebBoard().getState());
    getDeviceFlags().setConfigured(getRpctFebBoard().isConfigured());
    getDeviceFlags().setBroken(getRpctFebBoard().isBroken());
    getDeviceFlags().setWarm(getRpctFebBoard().isWarm());
    getDeviceFlags().setDisabled(getRpctFebBoard().isDisabled());
#endif // OFFLINE_ANALYSIS
    for (std::vector<FebPart *>::const_iterator _febpart = febparts_.begin()
             ; _febpart != febparts_.end() ; ++_febpart)
        if (*_febpart)
            (*_febpart)->configure(_configuration, _mask);
}

void FebBoard::imonitor(hwd::DeviceFlagsMask const & _mask)
{
#ifndef OFFLINE_ANALYSIS
    volatile bool _stop(false);
    bool _publish = false;
    try {
        getRpctFebBoard().monitor(&_stop, _mask.includesDisabled() ? ConfigurationFlags::INCLUDE_DISABLED : 0x0);
        _publish = true;
    } catch(std::exception & _e) {
        LOG4CPLUS_WARN(logger_, "Exception trying to monitor FebBoard " << name_ << ": " << _e.what());
    }
    getDeviceState().setState(getRpctFebBoard().getState());
    getDeviceFlags().setConfigured(getRpctFebBoard().isConfigured());
    getDeviceFlags().setBroken(getRpctFebBoard().isBroken());
    getDeviceFlags().setWarm(getRpctFebBoard().isWarm());
    getDeviceFlags().setDisabled(getRpctFebBoard().isDisabled());
    if (_publish)
#endif // OFFLINE_ANALYSIS
        for (std::vector<FebPart *>::const_iterator _febpart = febparts_.begin()
                 ; _febpart != febparts_.end() ; ++_febpart)
            if (*_febpart)
                (*_febpart)->monitor(_mask);
}

void FebBoard::imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask)
{
#ifndef OFFLINE_ANALYSIS
    rpct::IMonitorable::MonitorItems _rpct_monitoritems;
    for (std::vector<FebPart *>::const_iterator _febpart = febparts_.begin()
             ; _febpart != febparts_.end() ; ++_febpart)
        if (*_febpart && _mask.matches((*_febpart)->getDeviceFlags())) {
            if (_observables.hasDeviceObservables(*(*_febpart))) {
                hwd::DeviceObservables const & _deviceobservables = _observables.getDeviceObservables(*(*_febpart));
                if (_deviceobservables.hasParameter(febpart_type_.getTemperatureParameter()))
                    _rpct_monitoritems.push_back(rpct::monitoritem::feb::temp_);
            }
            std::vector<FebChip *> const & _febchips = (*_febpart)->getFebChips();
            std::vector<FebChip *>::const_iterator _febchip_end = _febchips.end();
            for (std::vector<FebChip *>::const_iterator _febchip = _febchips.begin()
                     ; _febchip != _febchip_end ; ++_febchip)
                if (*_febchip && _mask.matches((*_febchip)->getDeviceFlags())
                    && _observables.hasDeviceObservables(*(*_febchip))) {
                    hwd::DeviceObservables const & _deviceobservables = _observables.getDeviceObservables(*(*_febchip));
                    if (_deviceobservables.hasParameter(febchip_type_.getVThReadParameter()))
                        _rpct_monitoritems.push_back(rpct::monitoritem::feb::vth_);
                    if (_deviceobservables.hasParameter(febchip_type_.getVMonReadParameter()))
                        _rpct_monitoritems.push_back(rpct::monitoritem::feb::vmon_);
                }
        }
    bool _publish = false;
    try {
        getRpctFebBoard().monitor(_rpct_monitoritems, _mask.includesDisabled() ? ConfigurationFlags::INCLUDE_DISABLED : 0x0);
        _publish = true;
    } catch(std::exception & _e) {
        LOG4CPLUS_WARN(logger_, "Exception trying to monitor FebBoard " << name_ << ": " << _e.what());
    }
    getDeviceState().setState(getRpctFebBoard().getState());
    getDeviceFlags().setConfigured(getRpctFebBoard().isConfigured());
    getDeviceFlags().setBroken(getRpctFebBoard().isBroken());
    getDeviceFlags().setWarm(getRpctFebBoard().isWarm());
    getDeviceFlags().setDisabled(getRpctFebBoard().isDisabled());
    if (_publish)
#endif // OFFLINE_ANALYSIS
        for (std::vector<FebPart *>::const_iterator _febpart = febparts_.begin()
                 ; _febpart != febparts_.end() ; ++_febpart)
            if (*_febpart)
                (*_febpart)->monitor(_observables, _mask);
}

void FebBoard::iassume(hwd::Configuration const & _configuration)
{
    for (std::vector<FebPart *>::const_iterator _febpart = febparts_.begin()
             ; _febpart != febparts_.end() ; ++_febpart)
        if (*_febpart)
            (*_febpart)->assume(_configuration);
}

} // namespace fct
} // namespace rpct
