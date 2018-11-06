#include "rpct/hwd/System.h"

#include <stdexcept>
#include <sstream>

#include "rpct/hwd/HardwareStorage.h"
#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/Configuration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"
#include "rpct/hwd/Snapshot.h"

namespace rpct {
namespace hwd {

System::DeviceTypeCreatorBase::DeviceTypeCreatorBase()
{}

System::DeviceTypeCreatorBase::~DeviceTypeCreatorBase()
{}

DeviceType *
System::DeviceTypeCreatorBase::create(System & _system
                                      , integer_type _id
                                      , std::string const & _name) const
{
    return new DeviceType(_system, _id, _name);
}

System::DeviceCreatorBase::DeviceCreatorBase()
{}

System::DeviceCreatorBase::~DeviceCreatorBase()
{}

Device *
System::DeviceCreatorBase::create(System & _system
                                  , integer_type _id
                                  , DeviceType const & _devicetype
                                  , DeviceConfiguration const & _properties) const
{
    return new Device(_system, _id, _devicetype, _properties);
}

System::System(HardwareStorage & _hardwarestorage
               , std::string const & _name)
    : Device()
    , name_(_name)
    , hardwarestorage_(&_hardwarestorage)
{
    system_ = this;
}

System::~System()
{
    for (std::map<std::string, DeviceTypeCreatorBase *>::iterator _it = devicetypecreators_.begin()
             ; _it != devicetypecreators_.end() ; ++_it)
        delete _it->second;
    for (std::map<std::string, DeviceCreatorBase *>::iterator _it = devicecreators_.begin()
             ; _it != devicecreators_.end() ; ++_it)
        delete _it->second;
    for (std::map<integer_type, Device *>::iterator _it = devices_.begin()
             ; _it != devices_.end() ; ++_it)
        if (_it->first != id_)
            delete _it->second;
    for (std::map<std::string, DeviceType *>::iterator _it = devicetypes_.begin()
             ; _it != devicetypes_.end() ; ++_it)
        delete _it->second;
}

void System::load(rpct::tools::Time const & _time)
{
    getHardwareStorage().loadSystem(*this, _time);
}

void System::setProperties(DeviceConfiguration const & _properties)
{
    assume(_properties);
}

DataType const & System::getDataType(std::string const & _name) const
{
    return getHardwareStorage().getDataType(_name);
}

DataType const & System::getDataType(integer_type _id) const
{
    return getHardwareStorage().getDataType(_id);
}

ParameterType & System::registerParameterType(std::string const & _name
                                              , DataType const & _datatype)
{
    return getHardwareStorage().registerParameterType(*this, _name, _datatype);
}

ParameterType const & System::getParameterType(std::string const & _name) const
{
    std::map<std::string, ParameterType>::const_iterator _it = parametertypes_.find(_name);
    if (_it != parametertypes_.end())
        return _it->second;
    throw std::out_of_range(std::string("ParameterType ") + _name + " not present in System.");
}

ParameterType & System::getParameterType(std::string const & _name)
{
    std::map<std::string, ParameterType>::iterator _it = parametertypes_.find(_name);
    if (_it != parametertypes_.end())
        return _it->second;
    return getHardwareStorage().getParameterType(*this, _name);
}

Service & System::registerService(std::string const & _name)
{
    return getHardwareStorage().registerService(*this, _name);
}

Service const & System::getService(std::string const & _name) const
{
    std::map<std::string, Service>::const_iterator _it = services_.find(_name);
    if (_it != services_.end())
        return _it->second;
    throw std::out_of_range(std::string("Service ") + _name + " not present in System.");
}

Service & System::getService(std::string const & _name)
{
    std::map<std::string, Service>::iterator _it = services_.find(_name);
    if (_it != services_.end())
        return _it->second;
    return getHardwareStorage().getService(*this, _name);
}

DeviceType & System::registerDeviceType(std::string const & _name)
{
    return getHardwareStorage().registerDeviceType(*this, _name);
}

DeviceType const & System::getDeviceType(std::string const & _name) const
{
    std::map<std::string, DeviceType *>::const_iterator _it = devicetypes_.find(_name);
    if (_it != devicetypes_.end())
        return *(_it->second);
    throw std::out_of_range(std::string("DeviceType ") + _name + " not present in System.");
}

DeviceType & System::getDeviceType(std::string const & _name)
{
    std::map<std::string, DeviceType *>::iterator _it = devicetypes_.find(_name);
    if (_it != devicetypes_.end())
        return *(_it->second);
    return getHardwareStorage().getDeviceType(*this, _name);
}

Device & System::registerDevice(DeviceType const & _devicetype, DeviceConfiguration const & _properties)
{
    return getHardwareStorage().registerDevice(*this, _devicetype, _properties);
}

Device const & System::getDevice(integer_type _id) const
{
    std::map<integer_type, Device *>::const_iterator _it = devices_.find(_id);
    if (_it != devices_.end())
        return *(_it->second);
    std::ostringstream _errormsg;
    _errormsg << "Device " << _id << " not present in System.";
    throw std::out_of_range(_errormsg.str());
}

Device & System::getDevice(integer_type _id)
{
    std::map<integer_type, Device *>::iterator _it = devices_.find(_id);
    if (_it != devices_.end())
        return *(_it->second);
    std::ostringstream _errormsg;
    _errormsg << "Device " << _id << " not present in System.";
    throw std::out_of_range(_errormsg.str());
}

std::vector<Device *> const & System::getDevices(DeviceType const & _devicetype) const
{
    std::map<integer_type, std::vector<Device *> >::const_iterator _it
        = devicetype_devices_.find(_devicetype.getId());
    if (_it != devicetype_devices_.end())
        return _it->second;
    throw std::out_of_range(std::string("No Devices of DeviceType ") + _devicetype.getName() + " present in System.");
}

Configuration System::registerConfiguration(std::string const & _name)
{
    return getHardwareStorage().registerConfiguration(*this, _name);
}

Configuration System::getConfiguration(std::string const & _name)
{
    return getHardwareStorage().getConfiguration(*this, _name);
}

StandaloneDeviceConfiguration System::registerDeviceTypeConfiguration(DeviceType const & _devicetype)
{
    return getHardwareStorage().registerDeviceTypeConfiguration(*this, _devicetype);
}

StandaloneDeviceConfiguration System::getDeviceTypeConfiguration(integer_type _id)
{
    return getHardwareStorage().getDeviceTypeConfiguration(*this, _id);
}

SnapshotType & System::registerSnapshotType(std::string const & _name)
{
    return getHardwareStorage().registerSnapshotType(*this, _name);
}

SnapshotType const & System::getSnapshotType(std::string const & _name) const
{
    std::map<std::string, SnapshotType>::const_iterator _it = snapshottypes_.find(_name);
    if (_it != snapshottypes_.end())
        return _it->second;
    throw std::out_of_range(std::string("SnapshotType ") + _name + " not present in System.");
}

SnapshotType & System::getSnapshotType(std::string const & _name)
{
    std::map<std::string, SnapshotType>::iterator _it = snapshottypes_.find(_name);
    if (_it != snapshottypes_.end())
        return _it->second;
    return getHardwareStorage().getSnapshotType(*this, _name);
}

void System::assume(Snapshot const & _snapshot)
{
    assume(_snapshot.getTime());
}

void System::assumeChanges(Snapshot const & _snapshot, Snapshot const & _previous)
{
    assumeChanges(_snapshot.getTime(), _previous.getTime());
}

void System::assume(rpct::tools::Time const & _snapshot)
{
    assumeChanges(_snapshot, rpct::tools::Time(0));
}

void System::assumeChanges(rpct::tools::Time const & _snapshot, rpct::tools::Time const & _previous)
{
    {
        Configuration _configuration(*this, -1, "Snapshot");
        getHardwareStorage().loadSnapshotConfiguration(_configuration, _snapshot, _previous);

        for (std::map<integer_type, Device *>::iterator _device = devices_.begin()
                 ; _device != devices_.end() ; ++_device)
            _device->second->assume(_configuration);
    }
    {
        Configuration _observables(*this, -1, "Snapshot");
        getHardwareStorage().loadSnapshotObservables(_observables, _snapshot, _previous);

        for (std::map<integer_type, Device *>::iterator _device = devices_.begin()
                 ; _device != devices_.end() ; ++_device)
            _device->second->assume(_observables);
    }
}

void System::takeSnapshot(SnapshotType const & _snapshottype
                          , rpct::tools::Time const & _time)
{
    getHardwareStorage().takeSnapshot(*this, _snapshottype, _time);
}

std::vector<Snapshot> System::getSnapshots(SnapshotType const & _snapshottype
                                           , rpct::tools::Time const & _start
                                           , rpct::tools::Time const & _stop) const
{
    return getHardwareStorage().getSnapshots(*this, _snapshottype, _start, _stop);
}

ParameterType & System::addParameterType(integer_type _id
                                         , std::string const & _name, DataType const & _datatype)
{
    return (parametertypes_.insert(std::pair<std::string, ParameterType>(_name
                                                                         , ParameterType(_id
                                                                                         , _name
                                                                                         , _datatype)))
            ).first->second;
}

Service & System::addService(integer_type _id, std::string const & _name)
{
    return (services_.insert(std::pair<std::string, Service>(_name
                                                             , Service(_id, _name)))
            ).first->second;
}

DeviceType & System::addDeviceType(integer_type _id, std::string const & _name)
{
    { // make sure we don't create new without using it
        std::map<std::string, DeviceType *>::iterator _it = devicetypes_.find(_name);
        if (_it != devicetypes_.end())
            return *(_it->second);
    }

    std::map<std::string, DeviceTypeCreatorBase *>::iterator _devicetypecreator
        = devicetypecreators_.find(_name);
    devicetype_devices_[_id];
    if (_devicetypecreator != devicetypecreators_.end())
        return (*(devicetypes_.insert(std::pair<std::string
                                      , DeviceType *>(_name
                                                      , _devicetypecreator->second->create(*this, _id, _name)))
                  ).first->second);
    return *((devicetypes_.insert(std::pair<std::string
                                  , DeviceType *>(_name
                                                  , new DeviceType(*this, _id, _name)))
              ).first->second);
}

Device & System::addDevice(integer_type _id
                           , DeviceType const & _devicetype, DeviceConfiguration const & _properties)
{
    { // make sure we don't create new without using it
        std::map<integer_type, Device *>::iterator _it = devices_.find(_id);
        if (_it != devices_.end())
            return *(_it->second);
    }

    // avoid using pointers to devicetypes of other systems
    DeviceType const * _devicetype_ptr = &_devicetype;
    if (_devicetype_ptr->getSystem() != *this)
        _devicetype_ptr = &(getDeviceType(_devicetype.getName()));

    std::map<std::string, DeviceCreatorBase *>::iterator _devicecreator
        = devicecreators_.find(_devicetype.getName());
    if (_devicecreator != devicecreators_.end())
        {
            Device & _device
                = *((devices_.insert(std::pair<integer_type
                                     , Device *>(_id
                                                 , _devicecreator->second->create(*this, _id, *_devicetype_ptr, _properties)))
                     ).first->second);
            devicetype_devices_[_devicetype_ptr->getId()].push_back(&_device);
            return _device;
        }
    Device & _device
        = *((devices_.insert(std::pair<integer_type
                             , Device *>(_id
                                         , new Device(*this, _id, *_devicetype_ptr, _properties)))
             ).first->second);
    devicetype_devices_[_devicetype_ptr->getId()].push_back(&_device);
    return _device;
}

SnapshotType & System::addSnapshotType(integer_type _id, std::string const & _name)
{
    return (snapshottypes_.insert(std::pair<std::string, SnapshotType>(_name
                                                                       , SnapshotType(_id, _name)))
            ).first->second;
}

} // namespace hwd
} // namespace rpct
