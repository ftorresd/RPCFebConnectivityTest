#ifndef rpct_hwd_DeviceConfiguration_inl_h
#define rpct_hwd_DeviceConfiguration_inl_h

#include "rpct/hwd/DeviceConfiguration.h"

#include "rpct/hwd/State.h"
#include "rpct/hwd/DeviceFlags.h"

#include "rpct/hwd/DataType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/System.h"
#include "rpct/hwd/HardwareStorage.h"
#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/Configuration.h"

#include <stdexcept>

namespace rpct
{
    namespace hwd
    {

        inline integer_type DeviceConfiguration::getId() const
        {
            return id_;
        }

        inline DeviceType const &DeviceConfiguration::getDeviceType() const
        {
            if (devicetype_)
                return *devicetype_;
            throw std::logic_error("Attempt to get DeviceType of an incomplete DeviceConfiguration");
        }

        inline Configuration const &DeviceConfiguration::getConfiguration() const
        {
            if (configuration_)
                return *configuration_;
            throw std::logic_error("Attempt to get Configuration of an incomplete DeviceConfiguration");
        }

        inline Configuration &DeviceConfiguration::getConfiguration()
        {
            if (configuration_)
                return *configuration_;
            throw std::logic_error("Attempt to get Configuration of an incomplete DeviceConfiguration");
        }

        inline System const &DeviceConfiguration::getSystem() const
        {
            return getConfiguration().getSystem();
        }

        inline System &DeviceConfiguration::getSystem()
        {
            return getConfiguration().getSystem();
        }

        template <typename t_cpp_type>
        inline void
        DeviceConfiguration::getParameterSetting(Parameter const &_parameter, t_cpp_type &_value) const
        {
            if (_parameter.getDataType() != storage_type<t_cpp_type>::datatype())
                throw std::logic_error("Requesting ParameterSetting for wrong type");
            std::map<integer_type, integer_type>::const_iterator _setting = parametersettings_.find(_parameter.getId());
            if (_setting == parametersettings_.end())
                throw std::out_of_range(std::string("Requesting ParameterSetting for unregistered parameter ") + _parameter.getParameterType().getName());

            storage_type<t_cpp_type>::set(getConfiguration().getParameterSetting<typename storage_type<t_cpp_type>::sqlite_type>(_setting->second), _value);
        }

        template <typename t_cpp_type>
        inline void
        DeviceConfiguration::registerParameterSetting(Parameter const &_parameter, t_cpp_type const &_value)
        {
            if (_parameter.getDataType() != storage_type<t_cpp_type>::datatype())
                throw std::logic_error(std::string("Registering ParameterSetting for wrong DataType (ParameterType ") + _parameter.getParameterType().getName() + ", DataType " + storage_type<t_cpp_type>::datatype().getName() + " instead of " + _parameter.getDataType().getName() + ")");
            if (_parameter.getDeviceType() != getDeviceType())
                throw std::logic_error(std::string("Registering ParameterSetting for different DeviceType (ParameterType ") + _parameter.getParameterType().getName() + ", DeviceType " + _parameter.getDeviceType().getName() + " instead of " + getDeviceType().getName() + ")");
            typename storage_type<t_cpp_type>::sqlite_type _sqlite_value = storage_type<t_cpp_type>::get(_value);
            getSystem().getHardwareStorage().registerParameterSetting(*this, _parameter, _sqlite_value);
        }

        inline void DeviceConfiguration::registerDeviceState(State const &_devicestate)
        {
            registerParameterSetting(getDeviceType().getDeviceStateParameter(), _devicestate.getState());
        }

        inline void DeviceConfiguration::registerDeviceFlags(DeviceFlags const &_deviceflags)
        {
            registerParameterSetting(getDeviceType().getDeviceFlagsParameter(), _deviceflags.getFlags());
        }

        inline bool DeviceConfiguration::operator<(DeviceConfiguration const &_deviceconfiguration) const
        {
            return (id_ < _deviceconfiguration.id_);
        }

        inline bool DeviceConfiguration::operator==(DeviceConfiguration const &_deviceconfiguration) const
        {
            return (id_ == _deviceconfiguration.id_);
        }

        inline bool DeviceConfiguration::operator!=(DeviceConfiguration const &_deviceconfiguration) const
        {
            return (id_ != _deviceconfiguration.id_);
        }

    } // namespace hwd
} // namespace rpct

#endif // rpct_hwd_DeviceConfiguration_inl_h
