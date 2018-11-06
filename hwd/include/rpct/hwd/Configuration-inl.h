#ifndef rpct_hwd_Configuration_inl_h
#define rpct_hwd_Configuration_inl_h

#include "rpct/hwd/Configuration.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline integer_type Configuration::getId() const
{
    return id_;
}

inline std::string const & Configuration::getName() const
{
    return name_;
}

inline System const & Configuration::getSystem() const
{
    if (system_)
        return *system_;
    throw std::logic_error("Attempt to get System of an incomplete Configuration");
}

inline System & Configuration::getSystem()
{
    if (system_)
        return *system_;
    throw std::logic_error("Attempt to get System of an incomplete Configuration");
}

template<typename t_sqlite_type>
inline typename return_type<t_sqlite_type>::type
Configuration::getParameterSetting(integer_type const & _id) const
{
    throw std::logic_error("Configuration::getParameterSetting(integer_type const & _id)"
                           " is only implemented for the types in the hwd::datatype namespace");
}

template<> inline return_type<integer_type>::type
Configuration::getParameterSetting<integer_type>(integer_type const & _id) const
{
    return _id;
}

template<> inline return_type<float_type>::type
Configuration::getParameterSetting<float_type>(integer_type const & _id) const
{
    std::map<integer_type, float_type>::const_iterator _setting = floatparametersettings_.find(_id);
    if (_setting == floatparametersettings_.end())
        throw std::out_of_range("Requesting float ParameterSetting for unregistered parameter");

    return _setting->second;
}

template<> inline return_type<text_type>::type
Configuration::getParameterSetting<text_type>(integer_type const & _id) const
{
    std::map<integer_type, text_type>::const_iterator _setting = textparametersettings_.find(_id);
    if (_setting == textparametersettings_.end())
        throw std::out_of_range("Requesting text ParameterSetting for unregistered parameter");

    return _setting->second;
}

template<> inline return_type<blob_type>::type
Configuration::getParameterSetting<blob_type>(integer_type const & _id) const
{
    std::map<integer_type, blob_type>::const_iterator _setting = blobparametersettings_.find(_id);
    if (_setting == blobparametersettings_.end())
        throw std::out_of_range("Requesting blob ParameterSetting for unregistered parameter");

    return _setting->second;
}

inline bool Configuration::hasDeviceConfiguration(integer_type _device_id) const
{
    std::map<integer_type, integer_type>::const_iterator _deviceconfiguration
        = deviceconfigurations_.find(_device_id);
    return (_deviceconfiguration != deviceconfigurations_.end());
}

inline DeviceConfiguration const & Configuration::getDeviceConfiguration(integer_type _device_id) const
{
    std::map<integer_type, integer_type>::const_iterator _deviceconfiguration
        = deviceconfigurations_.find(_device_id);
    if (_deviceconfiguration != deviceconfigurations_.end())
        return *(devicetypeconfigurations_.at(_deviceconfiguration->second));
    else
        throw std::out_of_range("Requesting DeviceConfiguration for unregistered Device");
}

inline DeviceConfiguration & Configuration::getDeviceConfiguration(integer_type _device_id)
{
    std::map<integer_type, integer_type>::iterator _deviceconfiguration
        = deviceconfigurations_.find(_device_id);
    if (_deviceconfiguration != deviceconfigurations_.end())
        return *(devicetypeconfigurations_.at(_deviceconfiguration->second));
    else
        throw std::out_of_range("Requesting DeviceConfiguration for unregistered Device");
}

inline bool
Configuration::hasDeviceTypeConfiguration(integer_type _deviceconfiguration_id) const
{
    return (devicetypeconfigurations_.find(_deviceconfiguration_id) != devicetypeconfigurations_.end());
}

inline DeviceConfiguration const &
Configuration::getDeviceTypeConfiguration(integer_type _deviceconfiguration_id) const
{
    std::map<integer_type, DeviceConfiguration *>::const_iterator _it
        = devicetypeconfigurations_.find(_deviceconfiguration_id);
    if (_it == devicetypeconfigurations_.end())
        throw std::out_of_range("Requesting unregistered DeviceTypeConfiguration");

    return *(_it->second);
}

inline DeviceConfiguration &
Configuration::getDeviceTypeConfiguration(integer_type _deviceconfiguration_id)
{
    std::map<integer_type, DeviceConfiguration *>::iterator _it
        = devicetypeconfigurations_.find(_deviceconfiguration_id);
    if (_it == devicetypeconfigurations_.end())
        throw std::out_of_range("Requesting unregistered DeviceTypeConfiguration");

    return *(_it->second);
}

inline bool Configuration::operator<(Configuration const & _configuration) const
{
    return (id_ < _configuration.id_);
}

inline bool Configuration::operator==(Configuration const & _configuration) const
{
    return (id_ == _configuration.id_);
}

inline bool Configuration::operator!=(Configuration const & _configuration) const
{
    return (id_ != _configuration.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Configuration_inl_h
