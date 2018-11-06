#ifndef rpct_hwd_System_inl_h
#define rpct_hwd_System_inl_h

#include "rpct/hwd/System.h"

#include "rpct/hwd/HardwareStorage.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

template<typename t_devicetype_type>
inline
System::DeviceTypeCreator<t_devicetype_type>::DeviceTypeCreator()
    : DeviceTypeCreatorBase()
{}

template<typename t_devicetype_type>
inline DeviceType *
System::DeviceTypeCreator<t_devicetype_type>::create(System & _system
                                                     , integer_type _id
                                                     , std::string const & _name) const
{
    return new devicetype_type(_system, _id, _name);
}

template<typename t_device_type>
inline
System::DeviceCreator<t_device_type>::DeviceCreator()
    : DeviceCreatorBase()
{}

template<typename t_device_type>
inline Device *
System::DeviceCreator<t_device_type>::create(System & _system
                                             , integer_type _id
                                             , DeviceType const & _devicetype
                                             , DeviceConfiguration const & _properties) const
{
    return new device_type(_system, _id, _devicetype, _properties);
}

inline std::string const & System::getName() const
{
    return name_;
}

inline HardwareStorage const & System::getHardwareStorage() const
{
    if (hardwarestorage_)
        return *hardwarestorage_;
    throw std::logic_error("Attempt to get HardwareStorage of an incomplete System");
}

inline HardwareStorage & System::getHardwareStorage()
{
    if (hardwarestorage_)
        return *hardwarestorage_;
    throw std::logic_error("Attempt to get HardwareStorage of an incomplete System");
}

inline void System::initialise()
{}

inline void System::setId(integer_type _id)
{
    id_ = _id;
    devices_.insert(std::pair<integer_type, Device *>(_id, this));
}

inline void System::setDeviceType(DeviceType const & _devicetype)
{
    devicetype_ = &_devicetype;
}

template<typename t_device_type, typename t_devicetype_type>
inline void
System::registerDeviceType(std::string const & _devicetype_name)
{
    if (devicecreators_.find(_devicetype_name) == devicecreators_.end())
        {
            devicecreators_.insert(std::pair<std::string
                                   , DeviceCreatorBase *>(_devicetype_name
                                                          , new DeviceCreator<t_device_type>()));
            if (devicetypecreators_.find(_devicetype_name) == devicetypecreators_.end())
                devicetypecreators_.insert(std::pair<std::string
                                           , DeviceTypeCreatorBase *>(_devicetype_name
                                                                      , new DeviceTypeCreator<t_devicetype_type>()));
        }
    else
        throw std::logic_error(std::string("Could not register DeviceType ")
                               + _devicetype_name + ", it was registered before.");
}

template<typename t_device_type>
inline void
System::registerDeviceType(std::string const & _devicetype_name)
{
    if (devicecreators_.find(_devicetype_name) == devicecreators_.end())
        devicecreators_.insert(std::pair<std::string
                               , DeviceCreatorBase *>(_devicetype_name
                                                      , new DeviceCreator<t_device_type>()));
    else
        throw std::logic_error(std::string("Could not register DeviceType ")
                               + _devicetype_name + ", it was registered before.");
}

template<typename t_cpp_value>
inline void System::publish(Device const & _device, Parameter const & _parameter, t_cpp_value const & _value
                            , rpct::tools::Time const & _time)
{
    getHardwareStorage().publish(_device, _parameter, _value, _time);
}

inline void System::publish(Device const & _device, DeviceConfiguration const & _deviceconfiguration
                            , rpct::tools::Time const & _time)
{
    getHardwareStorage().publish(_device, _deviceconfiguration, _time);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_System_inl_h
