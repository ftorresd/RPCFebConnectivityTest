#ifndef rpct_hwd_DeviceConfiguration_h
#define rpct_hwd_DeviceConfiguration_h

#include <map>

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class DeviceConfiguration
{
public:
    static DeviceConfiguration const default_;

public:
    DeviceConfiguration(Configuration & _configuration
                        , integer_type _id
                        , DeviceType const & _devicetype);
    DeviceConfiguration();
    virtual ~DeviceConfiguration();

    integer_type getId() const;
    DeviceType const & getDeviceType() const;

    Configuration const & getConfiguration() const;
    Configuration & getConfiguration();

    System const & getSystem() const;
    System & getSystem();

    bool hasParameterSetting(Parameter const & _parameter) const;

    template<typename t_cpp_type>
    void getParameterSetting(Parameter const & _parameter, t_cpp_type & _value) const;

    template<typename t_cpp_type>
    void registerParameterSetting(Parameter const & _parameter, t_cpp_type const & _value);

    void registerDeviceState(State const & _devicestate);
    void registerDeviceFlags(DeviceFlags const & _deviceflags);

    bool operator< (DeviceConfiguration const & _deviceconfiguration) const;
    bool operator==(DeviceConfiguration const & _deviceconfiguration) const;
    bool operator!=(DeviceConfiguration const & _deviceconfiguration) const;

protected:
    friend class HardwareStorage;

    void addParameterSetting(Parameter const & _parameter
                             , integer_type _value);
    void addParameterSetting(Parameter const & _parameter
                             , integer_type _id, float_type _value);
    void addParameterSetting(Parameter const & _parameter
                             , integer_type _id, text_type const & _value);
    void addParameterSetting(Parameter const & _parameter
                             , integer_type _id, blob_type const & _value);

protected:
    Configuration * configuration_;
    integer_type id_;
    DeviceType const * devicetype_;

    std::map<integer_type, integer_type> parametersettings_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/DeviceConfiguration-inl.h"

#endif // rpct_hwd_DeviceConfiguration_h
