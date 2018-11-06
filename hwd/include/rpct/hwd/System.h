#ifndef rpct_hwd_System_h
#define rpct_hwd_System_h

#include <string>
#include <map>
#include <vector>

#include "rpct/tools/Time.h"

#include "rpct/hwd/fwd.h"

#include "rpct/hwd/Device.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/SnapshotType.h"

namespace rpct {
namespace hwd {

class System
    : public Device
{
protected:
    class DeviceTypeCreatorBase
    {
    public:
        DeviceTypeCreatorBase();
        virtual ~DeviceTypeCreatorBase();

        virtual DeviceType * create(System & _system
                                    , integer_type _id
                                    , std::string const & _name) const;
    };

    template<typename t_devicetype_type>
    class DeviceTypeCreator
        : public DeviceTypeCreatorBase
    {
    protected:
        typedef t_devicetype_type devicetype_type;

    public:
        DeviceTypeCreator();

        DeviceType * create(System & _system
                            , integer_type _id
                            , std::string const & _name) const;
    };

    class DeviceCreatorBase
    {
    public:
        DeviceCreatorBase();
        virtual ~DeviceCreatorBase();

        virtual Device * create(System & _system
                                , integer_type _id
                                , DeviceType const & _devicetype
                                , DeviceConfiguration const & _properties) const;
    };

    template<typename t_device_type>
    class DeviceCreator
        : public DeviceCreatorBase
    {
    protected:
        typedef t_device_type device_type;

    public:
        DeviceCreator();

        Device * create(System & _system
                        , integer_type _id
                        , DeviceType const & _devicetype
                        , DeviceConfiguration const & _properties) const;
    };

public:
    System(HardwareStorage & _hardwarestorage
           , std::string const & _name);
    virtual ~System();

    std::string const & getName() const;

    HardwareStorage const & getHardwareStorage() const;
    HardwareStorage & getHardwareStorage();

    void initialise();

    void load(rpct::tools::Time const & _time = rpct::tools::Time());
    void setId(integer_type _id);
    void setDeviceType(DeviceType const & _devicetype);
    void setProperties(DeviceConfiguration const & _properties);

    template<typename t_device_type, typename t_devicetype_type>
    void registerDeviceType(std::string const & _devicetype_name);
    template<typename t_device_type>
    void registerDeviceType(std::string const & _devicetype_name);

    DataType const & getDataType(std::string const & _name) const;
    DataType const & getDataType(integer_type _id) const;

    ParameterType & registerParameterType(std::string const & _name
                                          , DataType const & _datatype);
    ParameterType const & getParameterType(std::string const & _name) const;
    ParameterType & getParameterType(std::string const & _name);

    Service & registerService(std::string const & _name);
    Service const & getService(std::string const & _name) const;
    Service & getService(std::string const & _name);

    DeviceType & registerDeviceType(std::string const & _name);
    using Device::getDeviceType;
    DeviceType const & getDeviceType(std::string const & _name) const;
    DeviceType & getDeviceType(std::string const & _name);

    Device & registerDevice(DeviceType const & _devicetype
                            , DeviceConfiguration const & _properties);
    Device const & getDevice(integer_type _id) const;
    Device & getDevice(integer_type _id);
    std::vector<Device *> const & getDevices(DeviceType const & _devicetype) const;

    Configuration registerConfiguration(std::string const & _name);
    Configuration getConfiguration(std::string const & _name);

    StandaloneDeviceConfiguration registerDeviceTypeConfiguration(DeviceType const & _devicetype);
    StandaloneDeviceConfiguration getDeviceTypeConfiguration(integer_type _id);
    void registerDeviceConfiguration(Configuration & _configuration
                                     , Device const & _device
                                     , DeviceConfiguration const & _devicetypeconfiguration);

    SnapshotType & registerSnapshotType(std::string const & _name);
    SnapshotType const & getSnapshotType(std::string const & _name) const;
    SnapshotType & getSnapshotType(std::string const & _name);

    void takeSnapshot(SnapshotType const & _snapshottype
                      , rpct::tools::Time const & _time = rpct::tools::Time());

    std::vector<Snapshot> getSnapshots(SnapshotType const & _snapshottype
                                       , rpct::tools::Time const & _start = rpct::tools::Time(0)
                                       , rpct::tools::Time const & _stop = rpct::tools::Time()) const;

    using Device::assume;
    void assume(Snapshot const & _snapshot);
    void assumeChanges(Snapshot const & _snapshot
                       , Snapshot const & _previous);
    void assume(rpct::tools::Time const & _snapshot);
    void assumeChanges(rpct::tools::Time const & _snapshot
                       , rpct::tools::Time const & _previous);

    template<typename t_cpp_value>
    void publish(Device const & _device, Parameter const & _parameter, t_cpp_value const & _value
                 , rpct::tools::Time const & _time = rpct::tools::Time());
    void publish(Device const & _device, DeviceConfiguration const & _deviceconfiguration
                 , rpct::tools::Time const & _time = rpct::tools::Time());

protected:
    friend class HardwareStorage;

    ParameterType & addParameterType(integer_type _id
                                     , std::string const & _name
                                     , DataType const & _datatype);
    Service & addService(integer_type _id
                         , std::string const & _name);
    DeviceType & addDeviceType(integer_type _id
                               , std::string const & _name);
    Device & addDevice(integer_type _id
                       , DeviceType const & _devicetype
                       , DeviceConfiguration const & _properties);
    SnapshotType & addSnapshotType(integer_type _id
                                   , std::string const & _name);

protected:
    std::string name_;
    HardwareStorage * hardwarestorage_;

    std::map<std::string, DeviceCreatorBase *> devicecreators_;
    std::map<std::string, DeviceTypeCreatorBase *> devicetypecreators_;

    std::map<std::string, ParameterType> parametertypes_;
    std::map<std::string, Service> services_;
    std::map<std::string, DeviceType *> devicetypes_;
    std::map<integer_type, Device *> devices_;
    std::map<integer_type, std::vector<Device *> > devicetype_devices_;
    std::map<std::string, SnapshotType> snapshottypes_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/System-inl.h"

#endif // rpct_hwd_System_h
