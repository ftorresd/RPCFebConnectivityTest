#include "rpct/FebConnectivityTest/LinkBoard.h"

#include "log4cplus/loggingmacros.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/Connector.h"
#include "rpct/hwd/Connection.h"
#include "rpct/hwd/DeviceObservables.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/System.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"
#include "rpct/hwd/DeviceFlagsMask.h"

#include "rpct/hwd/HardwareStorage.h"

#include "rpct/FebConnectivityTest/LinkBox.h"
#include "rpct/FebConnectivityTest/FebConnector.h"

#ifndef OFFLINE_ANALYSIS
#include "rpct/tools/Timer.h"
#include "rpct/devices/LinkBoard.h"
#include "rpct/diag/TDiagCtrl.h"
#include "rpct/diag/IHistoMgr.h"
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

log4cplus::Logger LinkBoard::logger_ = log4cplus::Logger::getInstance("FebConnectivityLinkBoard");
double const LinkBoard::picoseconds_per_window_unit_ = 25000./240.;

LinkBoardType::LinkBoardType(hwd::System & _system
                             , hwd::integer_type _id
                             , std::string const & _name)
    : hwd::DeviceType(_system, _id, _name)
    , duration_parameter_(0)
    , window_open_parameter_(0)
    , window_closed_parameter_(0)
    , counts_parameter_(0)
    , windowed_counts_parameter_(0)
    , run_pulser_parameter_(0)
    , pulser_data_parameter_(0)
{}

LinkBoardType & LinkBoardType::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<LinkBoard, LinkBoardType>("LinkBoard");
    hwd::DeviceType & _linkboardtype = _system.registerDeviceType("LinkBoard");

    hwd::ParameterType const & _rpcid_type
        =  _system.registerParameterType("RpcId", hwd::DataType::integer_);
    _linkboardtype.registerParameter(_rpcid_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _name_type
        =  _system.registerParameterType("Name", hwd::DataType::text_);
    _linkboardtype.registerParameter(_name_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _duration_type
        =  _system.registerParameterType("Duration", hwd::DataType::integer_);
    _linkboardtype.registerParameter(_duration_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _window_open_type
        =  _system.registerParameterType("WindowOpen", hwd::DataType::integer_);
    _linkboardtype.registerParameter(_window_open_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _window_closed_type
        =  _system.registerParameterType("WindowClosed", hwd::DataType::integer_);
    _linkboardtype.registerParameter(_window_closed_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _counts_type
        =  _system.registerParameterType("Counts", hwd::DataType::blob_);
    _linkboardtype.registerParameter(_counts_type, 0, hwd::Parameter::is_observable_);

    hwd::ParameterType const & _windowed_counts_type
        =  _system.registerParameterType("WindowedCounts", hwd::DataType::blob_);
    _linkboardtype.registerParameter(_windowed_counts_type, 0, hwd::Parameter::is_observable_);

    hwd::ParameterType const & _run_pulser_type
        =  _system.registerParameterType("RunPulser", hwd::DataType::integer_);
    _linkboardtype.registerParameter(_run_pulser_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _pulser_data_type
        =  _system.registerParameterType("PulserData", hwd::DataType::blob_);
    _linkboardtype.registerParameter(_pulser_data_type, 0, hwd::Parameter::is_configurable_);

    hwd::Service const & _crate
        = _system.registerService("Crate");
    _linkboardtype.registerConnector(_crate, 0, hwd::in_);

    hwd::Service const & _signal
        = _system.registerService("Signal");
    for (std::size_t _position = 0 ; _position < LinkBoard::nfebconnectors_ ; ++_position)
        _linkboardtype.registerConnector(_signal, _position + 1, hwd::in_);

    return dynamic_cast<LinkBoardType &>(_linkboardtype);
}

hwd::Parameter const & LinkBoardType::addParameter(hwd::integer_type _id
                                                   , hwd::ParameterType const & _parametertype
                                                   , hwd::integer_type _position
                                                   , unsigned char _properties)
{
    hwd::Parameter const & _parameter
        = hwd::DeviceType::addParameter(_id, _parametertype, _position, _properties);
    if (_parametertype.getName() == "Duration")
        duration_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "WindowOpen")
        window_open_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "WindowClosed")
        window_closed_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "Counts")
        counts_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "WindowedCounts")
        windowed_counts_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "RunPulser")
        run_pulser_parameter_ = &(_parameter);
    else if (_parametertype.getName() == "PulserData")
        pulser_data_parameter_ = &(_parameter);
    return _parameter;
}

LinkBoard::LinkBoard(hwd::System & _system
                     , hwd::integer_type _id
                     , hwd::DeviceType const & _devicetype
                     , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , rpcid_(0), name_("")
    , linkbox_(0), position_(0)
    , febconnectors_(nfebconnectors_, 0)
    , linkboard_type_(dynamic_cast<LinkBoardType const &>(_devicetype))
    , duration_(1, 0)
    , window_open_(0), window_closed_(max_window_)
    , counts_(nchannels_, 0)
    , windowed_counts_(nchannels_, 0)
    , run_pulser_(false), pulser_data_()
#ifndef OFFLINE_ANALYSIS
    , rpct_linkboard_(0)
    , rpct_syncoder_(0)
#endif // OFFLINE_ANALYSIS
{
    hwd::Parameter const & _rpcid = getDeviceType().getParameter("RpcId", 0);
    if (_properties.hasParameterSetting(_rpcid))
        _properties.getParameterSetting(_rpcid, rpcid_);
    hwd::Parameter const & _name = getDeviceType().getParameter("Name", 0);
    if (_properties.hasParameterSetting(_name))
        _properties.getParameterSetting(_name, name_);
}

LinkBoardType & LinkBoard::registerDeviceType(hwd::System & _system)
{
    return LinkBoardType::registerDeviceType(_system);
}

LinkBoard & LinkBoard::registerDevice(hwd::System & _system
                                      , int _rpcid
                                      , hwd::DeviceFlags const & _deviceflags
                                      , std::string const & _name
                                      , LinkBox & _linkbox
                                      , std::size_t _position)
{
    hwd::DeviceType const & _linkboardtype = _system.getDeviceType("LinkBoard");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _linkboardtype);

    _properties.registerParameterSetting(_linkboardtype.getParameter("RpcId", 0), _rpcid);
    _properties.registerParameterSetting(_linkboardtype.getParameter("Name", 0), _name);
    _properties.registerDeviceFlags(_deviceflags);

    hwd::Device & _linkboard = _system.registerDevice(_linkboardtype, _properties);

    hwd::Connection _lbconnection
        = _system.getHardwareStorage().registerConnection(_linkbox
                                                          , _linkbox.getDeviceType().getConnector("Crate", _position, hwd::out_)
                                                          , _linkboard
                                                          , _linkboardtype.getConnector("Crate", 0, hwd::in_));

    _linkbox.addConnection(_lbconnection);
    _linkboard.addConnection(_lbconnection);

    return dynamic_cast<LinkBoard &>(_linkboard);
}

double LinkBoard::getAvgCount(std::size_t _febconnector) const
{
    if (_febconnector < 1 || _febconnector > nfebconnectors_)
        throw std::out_of_range("No FebConnector available at this position");
    double _avg(0.);
    if (counts_.empty())
        return 0.;
    else if (febconnectors_.at(_febconnector - 1))
        {
            FebConnectorStrips const & _strips = febconnectors_.at(_febconnector - 1)->getStrips();
            int _npins(0);
            ::uint32_t const * _count = &(counts_.at(FebConnectorStrips::npins_ * (_febconnector - 1)));
            for (int _pin = 1 ; _pin <= FebConnectorStrips::npins_ ; ++_pin, ++_count)
                if (_strips.isPinActive(_pin))
                    {
                        _avg += *_count;
                        ++_npins;
                    }
            _avg /= (double)(_npins);
        }
    else
        {
            ::uint32_t const * _count = &(counts_.at(FebConnectorStrips::npins_ * (_febconnector - 1)));
            for (int _pin = 1 ; _pin <= FebConnectorStrips::npins_ ; ++_pin, ++_count)
                _avg += *_count;
            _avg /= (double)(FebConnectorStrips::npins_);
        }
    return _avg;
}

double LinkBoard::getAvgWindowedCount(std::size_t _febconnector) const
{
    if (_febconnector < 1 || _febconnector > nfebconnectors_)
        throw std::out_of_range("No FebConnector available at this position");
    double _avg(0.);
    if (windowed_counts_.empty())
        return 0.;
    else if (febconnectors_.at(_febconnector - 1))
        {
            FebConnectorStrips const & _strips = febconnectors_.at(_febconnector - 1)->getStrips();
            int _npins(0);
            ::uint32_t const * _windowed_count = &(windowed_counts_.at(FebConnectorStrips::npins_ * (_febconnector - 1)));
            for (int _pin = 1 ; _pin <= FebConnectorStrips::npins_ ; ++_pin, ++_windowed_count)
                if (_strips.isPinActive(_pin))
                    {
                        _avg += *_windowed_count;
                        ++_npins;
                    }
            _avg /= (double)(_npins);
        }
    else
        {
            ::uint32_t const * _windowed_count = &(windowed_counts_.at(FebConnectorStrips::npins_ * (_febconnector - 1)));
            for (int _pin = 1 ; _pin <= FebConnectorStrips::npins_ ; ++_pin, ++_windowed_count)
                _avg += *_windowed_count;
            _avg /= (double)(FebConnectorStrips::npins_);
        }
    return _avg;
}

void LinkBoard::addConnection(hwd::Connection & _connection)
{
    if (_connection.getSourceConnector().getService().getName() == "Crate")
        {
            linkbox_ = (LinkBox *)(&(_connection.getSource()));
            position_ = _connection.getSourceConnector().getPosition();
        }
    else // Signal
        febconnectors_.at(_connection.getDestinationConnector().getPosition() - 1)
            = (FebConnector *)(&(_connection.getSource()));
}

void LinkBoard::iconfigure(hwd::DeviceConfiguration const & _deviceconfiguration, hwd::DeviceFlagsMask const & _mask)
{
    // Window
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getWindowOpenParameter()))
        {
            _deviceconfiguration.getParameterSetting(linkboard_type_.getWindowOpenParameter(), window_open_);
            if (window_open_ > max_window_)
                window_open_ = max_window_;
        }
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getWindowClosedParameter()))
        {
            _deviceconfiguration.getParameterSetting(linkboard_type_.getWindowClosedParameter(), window_closed_);
            if (window_closed_ > max_window_)
                window_closed_ = max_window_;
        }

#ifndef OFFLINE_ANALYSIS
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getWindowOpenParameter())
        || _deviceconfiguration.hasParameterSetting(linkboard_type_.getWindowClosedParameter()))
        {
            try {
                getRpctLinkBoard().setWindow(window_open_, window_closed_);
            } catch(std::exception & _e) {
                LOG4CPLUS_WARN(logger_, "Exception trying to configure LinkBoard " << name_ << ": " << _e.what());
            }
        }
#endif // OFFLINE_ANALYSIS

    // Pulser
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getPulserDataParameter()))
        _deviceconfiguration.getParameterSetting(linkboard_type_.getPulserDataParameter(), pulser_data_);
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getRunPulserParameter()))
        {
            _deviceconfiguration.getParameterSetting(linkboard_type_.getRunPulserParameter(), run_pulser_);
            if (run_pulser_)
                run_pulser_ = !(pulser_data_.empty());
#ifndef OFFLINE_ANALYSIS
            if (run_pulser_)
                {
                    try {
                        std::vector<rpct::BigInteger> _bi_pulser_data(pulser_data_.begin(), pulser_data_.end());
                        getRpctSynCoder().getPulser().Configure(_bi_pulser_data, _bi_pulser_data.size(), 0, 0);
                        ::uint64_t _pulser_limit = 0xfffffffffffffffull - 1;
                        rpct::TDiagCtrl & _diagctrl(getRpctSynCoder().getPulserDiagCtrl());
                        _diagctrl.Reset();
                        _diagctrl.Configure(_pulser_limit, IDiagCtrl::ttManual, 0);
                        _diagctrl.Start();
                    } catch(std::exception & _e) {
                        LOG4CPLUS_WARN(logger_, "Exception trying to configure LinkBoard " << name_ << " Pulsers: " << _e.what());
                    }
                }
            else
                {
                    try {
                        rpct::TDiagCtrl & _diagctrl(getRpctSynCoder().getPulserDiagCtrl());
                        if (_diagctrl.GetState() == rpct::IDiagCtrl::sRunning)
                            _diagctrl.Stop();
                    } catch(std::exception & _e) {
                        LOG4CPLUS_WARN(logger_, "Exception trying to configure LinkBoard " << name_ << " Pulsers: " << _e.what());
                    }
                }
#endif // OFFLINE_ANALYSIS
        }

    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getDurationParameter()))
        {
            ::int64_t _duration(0);
            _deviceconfiguration.getParameterSetting(linkboard_type_.getDurationParameter(), _duration);
            duration_.set(0, _duration * nanoseconds_per_bx_);
#ifndef OFFLINE_ANALYSIS
            if (_duration > 0)
                {
                    try {
                        rpct::TDiagCtrl & _diagctrl(getRpctSynCoder().getStatisticsDiagCtrl());
                        _diagctrl.Reset();
                        _diagctrl.Configure(_duration, IDiagCtrl::ttManual, 0);
                        _diagctrl.Start();
                    } catch(std::exception & _e) {
                        LOG4CPLUS_WARN(logger_, "Exception trying to configure LinkBoard " << name_ << " Counters: " << _e.what());
                    }
                }
#endif // OFFLINE_ANALYSIS
        }
}

void LinkBoard::imonitor(hwd::DeviceObservables const & _deviceobservables, hwd::DeviceFlagsMask const & _mask)
{
#ifndef OFFLINE_ANALYSIS
    bool _ready = false;

    if (_deviceobservables.hasParameter(linkboard_type_.getCountsParameter()))
        {
            try {
                rpct::TDiagCtrl & _diagctrl(getRpctSynCoder().getStatisticsDiagCtrl());
                if (_diagctrl.GetState() == rpct::IDiagCtrl::sRunning)
                    {
                        _ready = _diagctrl.CheckCountingEnded();
                        rpct::tools::Timer _timer(0, 100000000);
                        while (!_ready && _diagctrl.GetState() == rpct::IDiagCtrl::sRunning)
                            {
                                _timer.reset();
                                _timer.sleep();
                                _ready = _diagctrl.CheckCountingEnded();
                            }
                        _diagctrl.Stop();

                        rpct::IHistoMgr & _histo = getRpctSynCoder().getFullRateHistoMgr();
                        _histo.Refresh();
                        counts_.assign(_histo.GetLastData()
                                       , _histo.GetLastData() + _histo.GetBinCount());
                        getSystem().publish(*this, linkboard_type_.getCountsParameter(), counts_);
                    }
                else
                    throw std::logic_error("Attempt to monitor LinkBoard before configuring it.");
            } catch(std::exception & _e) {
                LOG4CPLUS_WARN(logger_, "Exception trying to monitor LinkBoard " << name_ << " at Counters: " << _e.what());
            }
        }

    if (_deviceobservables.hasParameter(linkboard_type_.getWindowedCountsParameter()))
        {
            try {
                rpct::TDiagCtrl & _diagctrl(getRpctSynCoder().getStatisticsDiagCtrl());
                if (_ready)
                    { // both rates
                        rpct::IHistoMgr & _histo = getRpctSynCoder().getWinRateHistoMgr();
                        _histo.Refresh();
                        windowed_counts_.assign(_histo.GetLastData()
                                                , _histo.GetLastData() + _histo.GetBinCount());
                        getSystem().publish(*this, linkboard_type_.getWindowedCountsParameter(), windowed_counts_);
                    }
                else if (_diagctrl.GetState() == rpct::IDiagCtrl::sRunning)
                    { // only windowed rate
                        bool _ready = _diagctrl.CheckCountingEnded();
                        rpct::tools::Timer _timer(0, 100000000);
                        while (!_ready && _diagctrl.GetState() == rpct::IDiagCtrl::sRunning)
                            {
                                _timer.reset();
                                _timer.sleep();
                                _ready = _diagctrl.CheckCountingEnded();
                            }
                        _diagctrl.Stop();

                        rpct::IHistoMgr & _histo = getRpctSynCoder().getWinRateHistoMgr();
                        _histo.Refresh();
                        windowed_counts_.assign(_histo.GetLastData()
                                                , _histo.GetLastData() + _histo.GetBinCount());
                        getSystem().publish(*this, linkboard_type_.getWindowedCountsParameter(), windowed_counts_);
                    }
                else
                    throw std::logic_error("Attempt to monitor LinkBoard before configuring it.");
            } catch(std::exception & _e) {
                LOG4CPLUS_WARN(logger_, "Exception trying to monitor LinkBoard " << name_ << ": " << _e.what());
            }
        }
#endif // OFFLINE_ANALYSIS
}

void LinkBoard::iassume(hwd::Configuration const & _configuration)
{
    for (std::vector<FebConnector *>::const_iterator _febconnector = febconnectors_.begin()
             ; _febconnector != febconnectors_.end() ; ++_febconnector)
        if (*_febconnector)
            (*_febconnector)->assume(_configuration);
}

void LinkBoard::iassume(hwd::DeviceConfiguration const & _deviceconfiguration)
{
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getDurationParameter()))
        {
            ::int64_t _duration;
            _deviceconfiguration.getParameterSetting(linkboard_type_.getDurationParameter(), _duration);
            duration_.set(0, _duration * nanoseconds_per_bx_);
        }
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getWindowOpenParameter()))
        {
            _deviceconfiguration.getParameterSetting(linkboard_type_.getWindowOpenParameter(), window_open_);
            if (window_open_ > max_window_)
                window_open_ = max_window_;
        }
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getWindowClosedParameter()))
        {
            _deviceconfiguration.getParameterSetting(linkboard_type_.getWindowClosedParameter(), window_closed_);
            if (window_closed_ > max_window_)
                window_closed_ = max_window_;
        }
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getPulserDataParameter()))
        _deviceconfiguration.getParameterSetting(linkboard_type_.getPulserDataParameter(), pulser_data_);
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getRunPulserParameter()))
        {
            _deviceconfiguration.getParameterSetting(linkboard_type_.getRunPulserParameter(), run_pulser_);
            if (run_pulser_)
                run_pulser_ = !(pulser_data_.empty());
        }

    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getCountsParameter()))
        _deviceconfiguration.getParameterSetting(linkboard_type_.getCountsParameter(), counts_);
    if (_deviceconfiguration.hasParameterSetting(linkboard_type_.getWindowedCountsParameter()))
        _deviceconfiguration.getParameterSetting(linkboard_type_.getWindowedCountsParameter(), windowed_counts_);
}

} // namespace fct
} // namespace rpct
