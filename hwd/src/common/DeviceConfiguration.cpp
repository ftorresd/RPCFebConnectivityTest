#include "rpct/hwd/DeviceConfiguration.h"

#include "rpct/hwd/DataType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/Configuration.h"

#include <string>
#include <stdexcept>

namespace rpct {
namespace hwd {

DeviceConfiguration const DeviceConfiguration::default_ = DeviceConfiguration();

DeviceConfiguration::DeviceConfiguration(Configuration & _configuration
                                         , integer_type _id
                                         , DeviceType const & _devicetype)
    : configuration_(&(_configuration))
    , id_(_id)
    , devicetype_(&_devicetype)
{}

DeviceConfiguration::DeviceConfiguration()
    : configuration_(0)
    , id_(-1)
    , devicetype_(&(DeviceType::default_))
{}

DeviceConfiguration::~DeviceConfiguration()
{}

bool DeviceConfiguration::hasParameterSetting(Parameter const & _parameter) const
{
    std::map<integer_type, integer_type>::const_iterator _setting
        = parametersettings_.find(_parameter.getId());
    return (_setting != parametersettings_.end());
}

void DeviceConfiguration::addParameterSetting(Parameter const & _parameter
                                              , integer_type _value)
{
    if (_parameter.getDeviceType() != getDeviceType())
        throw std::logic_error("Adding ParameterSetting for different DeviceType");
    if (_parameter.getDataType() != DataType::integer_)
        throw std::logic_error("Adding integer ParameterSetting for different DataType");
    parametersettings_.insert(std::pair<integer_type, integer_type>(_parameter.getId(), _value));
}

void DeviceConfiguration::addParameterSetting(Parameter const & _parameter
                                              , integer_type _id, float_type _value)
{
    if (_parameter.getDeviceType() != getDeviceType())
        throw std::logic_error("Adding ParameterSetting for different DeviceType");
    if (_parameter.getDataType() != DataType::float_)
        throw std::logic_error("Adding float ParameterSetting for different DataType");
    getConfiguration().addParameterSetting(_id, _value);
    parametersettings_.insert(std::pair<integer_type, integer_type>(_parameter.getId(), _id));
}

void DeviceConfiguration::addParameterSetting(Parameter const & _parameter
                                              , integer_type _id, text_type const & _value)
{
    if (_parameter.getDeviceType() != getDeviceType())
        throw std::logic_error("Adding ParameterSetting for different DeviceType");
    if (_parameter.getDataType() != DataType::text_)
        throw std::logic_error("Adding text ParameterSetting for different DataType");
    getConfiguration().addParameterSetting(_id, _value);
    parametersettings_.insert(std::pair<integer_type, integer_type>(_parameter.getId(), _id));
}

void DeviceConfiguration::addParameterSetting(Parameter const & _parameter
                                              , integer_type _id, blob_type const & _value)
{
    if (_parameter.getDeviceType() != getDeviceType())
        throw std::logic_error("Adding ParameterSetting for different DeviceType");
    if (_parameter.getDataType() != DataType::blob_)
        throw std::logic_error("Adding blob ParameterSetting for different DataType");
    getConfiguration().addParameterSetting(_id, _value);
    parametersettings_.insert(std::pair<integer_type, integer_type>(_parameter.getId(), _id));
}

} // namespace hwd
} // namespace rpct
