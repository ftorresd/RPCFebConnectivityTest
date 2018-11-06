#ifndef rpct_hwd_Configuration_h
#define rpct_hwd_Configuration_h

#include <string>
#include <map>

#include "rpct/hwd/fwd.h"

#include "rpct/tools/Time.h"

namespace rpct {
namespace hwd {

template<typename t_datatype_type>
struct return_type
{
    typedef t_datatype_type const & type;
};

template<>
struct return_type<float_type>
{
    typedef float_type type;
};

template<>
struct return_type<integer_type>
{
    typedef integer_type type;
};

class Configuration
{
public:
    static Configuration const default_;

protected:
    friend class DeviceConfiguration;
    friend class StandaloneDeviceConfiguration;

    void addParameterSetting(integer_type _id, float_type _value);
    void addParameterSetting(integer_type _id, text_type const & _value);
    void addParameterSetting(integer_type _id, blob_type const & _value);

    template<typename t_sqlite_type>
    typename return_type<t_sqlite_type>::type
    getParameterSetting(integer_type const & _id) const;

public:
    Configuration(System & _system
                  , integer_type _id
                  , std::string const & _name);
    Configuration();
    virtual ~Configuration();

    integer_type getId() const;
    std::string const & getName() const;

    System const & getSystem() const;
    System & getSystem();

    void load(rpct::tools::Time const & _time = rpct::tools::Time());
    void reset();

    bool hasDeviceConfiguration(Device const & _device) const;
    bool hasDeviceConfiguration(integer_type _device_id) const;
    DeviceConfiguration const & getDeviceConfiguration(Device const & _device) const;
    DeviceConfiguration const & getDeviceConfiguration(integer_type _device_id) const;
    DeviceConfiguration & getDeviceConfiguration(Device const & _device);
    DeviceConfiguration & getDeviceConfiguration(integer_type _device_id);

    bool hasDeviceTypeConfiguration(DeviceConfiguration const & _deviceconfiguration) const;
    bool hasDeviceTypeConfiguration(integer_type _devicetypeconfiguration_id) const;
    DeviceConfiguration const & getDeviceTypeConfiguration(integer_type _deviceconfiguration_id) const;
    DeviceConfiguration & getDeviceTypeConfiguration(integer_type _deviceconfiguration_id);

    DeviceConfiguration & registerDeviceTypeConfiguration(DeviceType const & _devicetype);
    void registerDeviceConfiguration(Device const & _device
                                     , DeviceConfiguration const & _deviceconfiguration);

    bool operator< (Configuration const & _configuration) const;
    bool operator==(Configuration const & _configuration) const;
    bool operator!=(Configuration const & _configuration) const;

protected:
    friend class HardwareStorage;

    DeviceConfiguration & addDeviceTypeConfiguration(integer_type _id
                                                     , DeviceType const & _devicetype);
    void addDeviceConfiguration(Device const & _device
                                , DeviceConfiguration const & _deviceconfiguration);
    void addDeviceConfiguration(integer_type _device_id
                                , integer_type _deviceconfiguration_id);

protected:
    System * system_;
    integer_type id_;
    std::string name_;

    std::map<integer_type, DeviceConfiguration *> devicetypeconfigurations_;
    std::map<integer_type, integer_type> deviceconfigurations_;

    // for DeviceConfiguration:
    std::map<integer_type, float_type> floatparametersettings_;
    std::map<integer_type, text_type> textparametersettings_;
    std::map<integer_type, blob_type> blobparametersettings_;
};

template<> return_type<integer_type>::type
Configuration::getParameterSetting<integer_type>(integer_type const & _id) const;
template<> return_type<float_type>::type
Configuration::getParameterSetting<float_type>(integer_type const & _id) const;
template<> return_type<text_type>::type
Configuration::getParameterSetting<text_type>(integer_type const & _id) const;
template<> return_type<blob_type>::type
Configuration::getParameterSetting<blob_type>(integer_type const & _id) const;

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/Configuration-inl.h"

#endif // rpct_hwd_Configuration_h
