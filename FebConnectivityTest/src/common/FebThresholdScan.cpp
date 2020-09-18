#include "rpct/FebConnectivityTest/FebThresholdScan.h"

#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/FebConnectivityTest/LinkBoard.h"

namespace rpct
{
    namespace fct
    {

        FebThresholdScan::FebThresholdScan(hwd::System &_system, hwd::integer_type _id, hwd::DeviceType const &_devicetype, hwd::DeviceConfiguration const &_properties)
            : hwd::Device(_system, _id, _devicetype, _properties), active_vth_(0)
        {
            hwd::Parameter const &_roll_selection_selected = getDeviceType().getParameter("RollSelectionSelected", 0);
            if (_properties.hasParameterSetting(_roll_selection_selected))
            {
                std::vector<tools::RollId> _selected;
                _properties.getParameterSetting(_roll_selection_selected, _selected);
                for (std::vector<tools::RollId>::const_iterator _roll = _selected.begin(); _roll != _selected.end(); ++_roll)
                    roll_selection_.select(*_roll);
            }
            hwd::Parameter const &_roll_selection_masked = getDeviceType().getParameter("RollSelectionMasked", 0);
            if (_properties.hasParameterSetting(_roll_selection_masked))
            {
                std::vector<tools::RollId> _masked;
                _properties.getParameterSetting(_roll_selection_masked, _masked);
                for (std::vector<tools::RollId>::const_iterator _roll = _masked.begin(); _roll != _masked.end(); ++_roll)
                    roll_selection_.mask(*_roll);
            }
            hwd::Parameter const &_count_duration_type = getDeviceType().getParameter("CountDuration", 0);
            if (_properties.hasParameterSetting(_count_duration_type))
            {
                ::int64_t _duration(0);
                _properties.getParameterSetting(_count_duration_type, _duration);
                count_duration_.set(0, _duration * rpct::fct::LinkBoard::nanoseconds_per_bx_);
            }
            hwd::Parameter const &_vth_min = getDeviceType().getParameter("VThMin", 0);
            if (_properties.hasParameterSetting(_vth_min))
                _properties.getParameterSetting(_vth_min, vth_min_);
            hwd::Parameter const &_vth_n_steps = getDeviceType().getParameter("VThNSteps", 0);
            if (_properties.hasParameterSetting(_vth_n_steps))
                _properties.getParameterSetting(_vth_n_steps, vth_n_steps_);
            hwd::Parameter const &_vth_step_size = getDeviceType().getParameter("VThStepSize", 0);
            if (_properties.hasParameterSetting(_vth_step_size))
                _properties.getParameterSetting(_vth_step_size, vth_step_size_);
            hwd::Parameter const &_vmon = getDeviceType().getParameter("VMon", 0);
            if (_properties.hasParameterSetting(_vmon))
                _properties.getParameterSetting(_vmon, vmon_);
            hwd::Parameter const &_auto_correct = getDeviceType().getParameter("AutoCorrect", 0);
            if (_properties.hasParameterSetting(_auto_correct))
                _properties.getParameterSetting(_auto_correct, auto_correct_);
            hwd::Parameter const &_use_pulser = getDeviceType().getParameter("UsePulser", 0);
            if (_properties.hasParameterSetting(_use_pulser))
                _properties.getParameterSetting(_use_pulser, use_pulser_);
            hwd::Parameter const &_include_disabled = getDeviceType().getParameter("IncludeDisabled", 0);
            if (_properties.hasParameterSetting(_include_disabled))
                _properties.getParameterSetting(_include_disabled, include_disabled_);

            hwd::Parameter const &_snapshot_name = getDeviceType().getParameter("SnapshotName", 0);
            if (_properties.hasParameterSetting(_snapshot_name))
                _properties.getParameterSetting(_snapshot_name, snapshot_name_);
        }

        hwd::DeviceType &FebThresholdScan::registerDeviceType(hwd::System &_system)
        {
            _system.registerDeviceType<FebThresholdScan>("FebThresholdScan");
            hwd::DeviceType &_febthresholdscantype = _system.registerDeviceType("FebThresholdScan");

            hwd::ParameterType const &_roll_selection_selected_type = _system.registerParameterType("RollSelectionSelected", hwd::DataType::blob_);
            _febthresholdscantype.registerParameter(_roll_selection_selected_type, 0, hwd::Parameter::is_property_);
            hwd::ParameterType const &_roll_selection_masked_type = _system.registerParameterType("RollSelectionMasked", hwd::DataType::blob_);
            _febthresholdscantype.registerParameter(_roll_selection_masked_type, 0, hwd::Parameter::is_property_);
            hwd::ParameterType const &_count_duration_type = _system.registerParameterType("CountDuration", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_count_duration_type, 0, hwd::Parameter::is_property_);

            hwd::ParameterType const &_vth_min_type = _system.registerParameterType("VThMin", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_vth_min_type, 0, hwd::Parameter::is_property_);
            hwd::ParameterType const &_vth_n_steps_type = _system.registerParameterType("VThNSteps", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_vth_n_steps_type, 0, hwd::Parameter::is_property_);
            hwd::ParameterType const &_vth_step_size_type = _system.registerParameterType("VThStepSize", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_vth_step_size_type, 0, hwd::Parameter::is_property_);
            hwd::ParameterType const &_vmon_type = _system.registerParameterType("VMon", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_vmon_type, 0, hwd::Parameter::is_property_);

            hwd::ParameterType const &_active_vth_type = _system.registerParameterType("ActiveVTh", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_active_vth_type, 0, hwd::Parameter::is_configurable_);

            hwd::ParameterType const &_auto_correct_type = _system.registerParameterType("AutoCorrect", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_auto_correct_type, 0, hwd::Parameter::is_property_);
            hwd::ParameterType const &_use_pulser_type = _system.registerParameterType("UsePulser", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_use_pulser_type, 0, hwd::Parameter::is_property_);
            hwd::ParameterType const &_include_disabled_type = _system.registerParameterType("IncludeDisabled", hwd::DataType::integer_);
            _febthresholdscantype.registerParameter(_include_disabled_type, 0, hwd::Parameter::is_property_);

            hwd::ParameterType const &_snapshot_name_type = _system.registerParameterType("SnapshotName", hwd::DataType::text_);
            _febthresholdscantype.registerParameter(_snapshot_name_type, 0, hwd::Parameter::is_property_);

            return _febthresholdscantype;
        }

        FebThresholdScan &FebThresholdScan::registerDevice(hwd::System &_system, rpct::tools::RollSelection const &_roll_selection, rpct::tools::Time const &_count_duration, unsigned int _vth_min, unsigned int _vth_n_steps, unsigned int _vth_step_size, unsigned int _vmon, bool _auto_correct, bool _use_pulser, bool _include_disabled, std::string const &_snapshot_name)
        {
            hwd::DeviceType const &_febthresholdscantype = _system.getDeviceType("FebThresholdScan");
            hwd::StandaloneDeviceConfiguration _properties = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _febthresholdscantype);

            {
                std::vector<tools::RollId> _selected(_roll_selection.getSelected().begin(), _roll_selection.getSelected().end());
                _properties.registerParameterSetting(_febthresholdscantype.getParameter("RollSelectionSelected", 0), _selected);
            }
            {
                std::vector<tools::RollId> _masked(_roll_selection.getMasked().begin(), _roll_selection.getMasked().end());
                _properties.registerParameterSetting(_febthresholdscantype.getParameter("RollSelectionMasked", 0), _masked);
            }

            {
                ::int64_t _duration = _count_duration.seconds() * rpct::fct::LinkBoard::bx_per_second_ + _count_duration.nanoseconds() / rpct::fct::LinkBoard::nanoseconds_per_bx_;
                _properties.registerParameterSetting(_febthresholdscantype.getParameter("CountDuration", 0), _duration);
            }

            _properties.registerParameterSetting(_febthresholdscantype.getParameter("VThMin", 0), _vth_min);
            _properties.registerParameterSetting(_febthresholdscantype.getParameter("VThNSteps", 0), _vth_n_steps);
            _properties.registerParameterSetting(_febthresholdscantype.getParameter("VThStepSize", 0), _vth_step_size);
            _properties.registerParameterSetting(_febthresholdscantype.getParameter("VMon", 0), _vmon);

            _properties.registerParameterSetting(_febthresholdscantype.getParameter("AutoCorrect", 0), _auto_correct);
            _properties.registerParameterSetting(_febthresholdscantype.getParameter("UsePulser", 0), _use_pulser);
            _properties.registerParameterSetting(_febthresholdscantype.getParameter("IncludeDisabled", 0), _include_disabled);

            _properties.registerParameterSetting(_febthresholdscantype.getParameter("SnapshotName", 0), _snapshot_name);

            hwd::Device &_febthresholdscan = _system.registerDevice(_febthresholdscantype, _properties);

            return dynamic_cast<FebThresholdScan &>(_febthresholdscan);
        }

        void FebThresholdScan::iconfigure(hwd::DeviceConfiguration const &_deviceconfiguration, hwd::DeviceFlagsMask const &_mask)
        {
            hwd::Parameter const &_active_vth = getDeviceType().getParameter("ActiveVTh", 0);
            if (_deviceconfiguration.hasParameterSetting(_active_vth))
                _deviceconfiguration.getParameterSetting(_active_vth, active_vth_);
        }

        void FebThresholdScan::iassume(hwd::DeviceConfiguration const &_deviceconfiguration)
        {
            hwd::Parameter const &_active_vth = getDeviceType().getParameter("ActiveVTh", 0);
            if (_deviceconfiguration.hasParameterSetting(_active_vth))
                _deviceconfiguration.getParameterSetting(_active_vth, active_vth_);
        }

    } // namespace fct
} // namespace rpct
