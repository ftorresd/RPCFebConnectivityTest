#include "rpct/hwd/Configuration.h"

#include "rpct/hwd/Device.h"
#include "rpct/hwd/System.h"
#include "rpct/hwd/HardwareStorage.h"
#include "rpct/hwd/DeviceConfiguration.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

Configuration const Configuration::default_ = Configuration();

void Configuration::addParameterSetting(integer_type _id, float_type _value)
{
    floatparametersettings_.insert(std::pair<integer_type, float_type>(_id, _value));
}

void Configuration::addParameterSetting(integer_type _id, text_type const & _value)
{
    textparametersettings_.insert(std::pair<integer_type, text_type>(_id, _value));
}

void Configuration::addParameterSetting(integer_type _id, blob_type const & _value)
{
    blobparametersettings_.insert(std::pair<integer_type, blob_type>(_id, _value));
}

Configuration::Configuration(System & _system
                             , integer_type _id
                             , std::string const & _name)
    : system_(&_system)
    , id_(_id)
    , name_(_name)
{}

Configuration::Configuration()
    : system_(0)
    , id_(-1)
    , name_("DefaultConfiguration")
{}

Configuration::~Configuration()
{
    reset();
}

void Configuration::load(rpct::tools::Time const & _time)
{
    getSystem().getHardwareStorage().loadConfiguration(*this, _time);
}

void Configuration::reset()
{
    for (std::map<integer_type, DeviceConfiguration *>::iterator _it = devicetypeconfigurations_.begin()
             ; _it != devicetypeconfigurations_.end() ; ++_it)
        delete _it->second;

    devicetypeconfigurations_.clear();;
    deviceconfigurations_.clear();
    floatparametersettings_.clear();
    textparametersettings_.clear();
    blobparametersettings_.clear();
}

bool Configuration::hasDeviceConfiguration(Device const & _device) const
{
    std::map<integer_type, integer_type>::const_iterator _deviceconfiguration
        = deviceconfigurations_.find(_device.getId());
    return (_deviceconfiguration != deviceconfigurations_.end());
}

DeviceConfiguration const & Configuration::getDeviceConfiguration(Device const & _device) const
{
    std::map<integer_type, integer_type>::const_iterator _deviceconfiguration
        = deviceconfigurations_.find(_device.getId());
    if (_deviceconfiguration != deviceconfigurations_.end())
        return *(devicetypeconfigurations_.at(_deviceconfiguration->second));
    else
        throw std::out_of_range("Requesting DeviceConfiguration for unregistered Device");
}

DeviceConfiguration & Configuration::getDeviceConfiguration(Device const & _device)
{
    std::map<integer_type, integer_type>::iterator _deviceconfiguration
        = deviceconfigurations_.find(_device.getId());
    if (_deviceconfiguration != deviceconfigurations_.end())
        return *(devicetypeconfigurations_.at(_deviceconfiguration->second));
    else
        throw std::out_of_range("Requesting DeviceConfiguration for unregistered Device");
}

bool Configuration::hasDeviceTypeConfiguration(DeviceConfiguration const & _deviceconfiguration) const
{
    return (devicetypeconfigurations_.find(_deviceconfiguration.getId()) != devicetypeconfigurations_.end());
}

DeviceConfiguration & Configuration::registerDeviceTypeConfiguration(DeviceType const & _devicetype)
{
    return getSystem().getHardwareStorage().registerDeviceTypeConfiguration(*this, _devicetype);
}

void Configuration::registerDeviceConfiguration(Device const & _device
                                                , DeviceConfiguration const & _deviceconfiguration)
{
    if (deviceconfigurations_.find(_device.getId()) == deviceconfigurations_.end())
        getSystem().getHardwareStorage().registerDeviceConfiguration(*this
                                                                     , _device
                                                                     , _deviceconfiguration);
}

DeviceConfiguration & Configuration::addDeviceTypeConfiguration(integer_type _id
                                                                , DeviceType const & _devicetype)
{
    return *(devicetypeconfigurations_.insert(std::pair<integer_type
                                              , DeviceConfiguration *>(_id
                                                                       , new DeviceConfiguration(*this
                                                                                                 , _id
                                                                                                 , _devicetype))
                                              ).first->second);
}

void Configuration::addDeviceConfiguration(Device const & _device
                                           , DeviceConfiguration const & _deviceconfiguration)
{
    if (devicetypeconfigurations_.find(_deviceconfiguration.getId()) != devicetypeconfigurations_.end())
        deviceconfigurations_.insert(std::pair<integer_type
                                     , integer_type>(_device.getId()
                                                     , _deviceconfiguration.getId()));
    else
        throw std::out_of_range("Requesting to add Device-DeviceConfiguration for unregistered DeviceConfiguration");
}

void Configuration::addDeviceConfiguration(integer_type _device_id
                                           , integer_type _deviceconfiguration_id)
{
    if (devicetypeconfigurations_.find(_deviceconfiguration_id) != devicetypeconfigurations_.end())
        deviceconfigurations_.insert(std::pair<integer_type
                                     , integer_type>(_device_id
                                                     , _deviceconfiguration_id));
    else
        throw std::out_of_range("Requesting to add Device-DeviceConfiguration for unregistered DeviceConfiguration");
}

} // namespace hwd
} // namespace rpct
