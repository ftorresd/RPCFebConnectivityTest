#include "rpct/FebConnectivityTest/FebConnectivityTest.h"

#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/FebConnectivityTest/LinkBoard.h"
#include "rpct/FebConnectivityTest/FebSystem.h"
#include "rpct/FebConnectivityTest/FebConnector.h"

namespace rpct {
namespace fct {

FebConnectivityTest::FebConnectivityTest(hwd::System & _system
                                         , hwd::integer_type _id
                                         , hwd::DeviceType const & _devicetype
                                         , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
{
    hwd::Parameter const & _roll_selection_selected
        = getDeviceType().getParameter("RollSelectionSelected", 0);
    if (_properties.hasParameterSetting(_roll_selection_selected))
        {
            std::vector<tools::RollId> _selected;
            _properties.getParameterSetting(_roll_selection_selected, _selected);
            for (std::vector<tools::RollId>::const_iterator _roll = _selected.begin()
                     ; _roll != _selected.end() ; ++_roll)
                roll_selection_.select(*_roll);
        }
    hwd::Parameter const & _roll_selection_masked
        = getDeviceType().getParameter("RollSelectionMasked", 0);
    if (_properties.hasParameterSetting(_roll_selection_masked))
        {
            std::vector<tools::RollId> _masked;
            _properties.getParameterSetting(_roll_selection_masked, _masked);
            for (std::vector<tools::RollId>::const_iterator _roll = _masked.begin()
                     ; _roll != _masked.end() ; ++_roll)
                roll_selection_.mask(*_roll);
        }
    hwd::Parameter const & _count_duration_type = getDeviceType().getParameter("CountDuration", 0);
    if (_properties.hasParameterSetting(_count_duration_type))
        {
            ::int64_t _duration(0);
            _properties.getParameterSetting(_count_duration_type, _duration);
            count_duration_.set(0, _duration * rpct::fct::LinkBoard::nanoseconds_per_bx_);
        }
    hwd::Parameter const & _vth_low = getDeviceType().getParameter("VThLow", 0);
    if (_properties.hasParameterSetting(_vth_low))
        _properties.getParameterSetting(_vth_low, vth_low_);
    hwd::Parameter const & _vth_high = getDeviceType().getParameter("VThHigh", 0);
    if (_properties.hasParameterSetting(_vth_high))
        _properties.getParameterSetting(_vth_high, vth_high_);
    hwd::Parameter const & _vmon = getDeviceType().getParameter("VMon", 0);
    if (_properties.hasParameterSetting(_vmon))
        _properties.getParameterSetting(_vmon, vmon_);
    hwd::Parameter const & _auto_correct = getDeviceType().getParameter("AutoCorrect", 0);
    if (_properties.hasParameterSetting(_auto_correct))
        _properties.getParameterSetting(_auto_correct, auto_correct_);
    hwd::Parameter const & _use_pulser = getDeviceType().getParameter("UsePulser", 0);
    if (_properties.hasParameterSetting(_use_pulser))
        _properties.getParameterSetting(_use_pulser, use_pulser_);
    hwd::Parameter const & _include_disabled = getDeviceType().getParameter("IncludeDisabled", 0);
    if (_properties.hasParameterSetting(_include_disabled))
        _properties.getParameterSetting(_include_disabled, include_disabled_);

    hwd::Parameter const & _snapshot_name = getDeviceType().getParameter("SnapshotName", 0);
    if (_properties.hasParameterSetting(_snapshot_name))
        _properties.getParameterSetting(_snapshot_name, snapshot_name_);
}

hwd::DeviceType & FebConnectivityTest::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<FebConnectivityTest>("FebConnectivityTest");
    hwd::DeviceType & _febconnectivitytesttype
        = _system.registerDeviceType("FebConnectivityTest");

    hwd::ParameterType const & _roll_selection_selected_type
        =  _system.registerParameterType("RollSelectionSelected", hwd::DataType::blob_);
    _febconnectivitytesttype.registerParameter(_roll_selection_selected_type, 0, hwd::Parameter::is_property_);
    hwd::ParameterType const & _roll_selection_masked_type
        =  _system.registerParameterType("RollSelectionMasked", hwd::DataType::blob_);
    _febconnectivitytesttype.registerParameter(_roll_selection_masked_type, 0, hwd::Parameter::is_property_);
    hwd::ParameterType const & _count_duration_type
        =  _system.registerParameterType("CountDuration", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_count_duration_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _vth_low_type
        =  _system.registerParameterType("VThLow", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_vth_low_type, 0, hwd::Parameter::is_property_);
    hwd::ParameterType const & _vth_high_type
        =  _system.registerParameterType("VThHigh", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_vth_high_type, 0, hwd::Parameter::is_property_);
    hwd::ParameterType const & _vmon_type
        =  _system.registerParameterType("VMon", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_vmon_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _active_febconnector_id_type
        =  _system.registerParameterType("ActiveFebConnectorId", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_active_febconnector_id_type, 0, hwd::Parameter::is_configurable_);

    hwd::ParameterType const & _auto_correct_type
        =  _system.registerParameterType("AutoCorrect", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_auto_correct_type, 0, hwd::Parameter::is_property_);
    hwd::ParameterType const & _use_pulser_type
        =  _system.registerParameterType("UsePulser", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_use_pulser_type, 0, hwd::Parameter::is_property_);
    hwd::ParameterType const & _include_disabled_type
        =  _system.registerParameterType("IncludeDisabled", hwd::DataType::integer_);
    _febconnectivitytesttype.registerParameter(_include_disabled_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _snapshot_name_type
        =  _system.registerParameterType("SnapshotName", hwd::DataType::text_);
    _febconnectivitytesttype.registerParameter(_snapshot_name_type, 0, hwd::Parameter::is_property_);

    return _febconnectivitytesttype;
}

FebConnectivityTest & FebConnectivityTest::registerDevice(hwd::System & _system
                                                          , rpct::tools::RollSelection const &  _roll_selection
                                                          , rpct::tools::Time const & _count_duration
                                                          , unsigned int _vth_low, unsigned int _vth_high
                                                          , unsigned int _vmon
                                                          , bool _auto_correct
                                                          , bool _use_pulser
                                                          , bool _include_disabled
                                                          , std::string const & _snapshot_name)
{
    hwd::DeviceType const & _febconnectivitytesttype = _system.getDeviceType("FebConnectivityTest");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _febconnectivitytesttype);


    {
        std::vector<tools::RollId> _selected(_roll_selection.getSelected().begin(), _roll_selection.getSelected().end());
        _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("RollSelectionSelected", 0), _selected);
    }
    {
        std::vector<tools::RollId> _masked(_roll_selection.getMasked().begin(), _roll_selection.getMasked().end());
        _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("RollSelectionMasked", 0), _masked);
    }

    {
        ::int64_t _duration = _count_duration.seconds() * rpct::fct::LinkBoard::bx_per_second_
            + _count_duration.nanoseconds() / rpct::fct::LinkBoard::nanoseconds_per_bx_;
        _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("CountDuration", 0), _duration);
    }

    _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("VThLow", 0), _vth_low);
    _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("VThHigh", 0), _vth_high);
    _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("VMon", 0), _vmon);

    _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("AutoCorrect", 0), _auto_correct);
    _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("UsePulser", 0), _use_pulser);
    _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("IncludeDisabled", 0), _include_disabled);

    _properties.registerParameterSetting(_febconnectivitytesttype.getParameter("SnapshotName", 0), _snapshot_name);

    hwd::Device & _febconnectivitytest = _system.registerDevice(_febconnectivitytesttype, _properties);

    return dynamic_cast<FebConnectivityTest &>(_febconnectivitytest);
}

FebConnector const & FebConnectivityTest::getActiveFebConnector() const
{
    return dynamic_cast<FebConnector const &>(getSystem().getDevice(active_febconnector_id_));
}

FebConnector & FebConnectivityTest::getActiveFebConnector()
{
    return dynamic_cast<FebConnector &>(getSystem().getDevice(active_febconnector_id_));
}

void FebConnectivityTest::iconfigure(hwd::DeviceConfiguration const & _deviceconfiguration, hwd::DeviceFlagsMask const & _mask)
{
    if (_deviceconfiguration.hasParameterSetting(getDeviceType().getParameter("ActiveFebConnectorId", 0)))
        _deviceconfiguration.getParameterSetting(getDeviceType().getParameter("ActiveFebConnectorId", 0), active_febconnector_id_);
}

void FebConnectivityTest::iassume(hwd::DeviceConfiguration const & _deviceconfiguration)
{
    if (_deviceconfiguration.hasParameterSetting(getDeviceType().getParameter("ActiveFebConnectorId", 0)))
        _deviceconfiguration.getParameterSetting(getDeviceType().getParameter("ActiveFebConnectorId", 0), active_febconnector_id_);
}


} // namespace fct
} // namespace rpct
