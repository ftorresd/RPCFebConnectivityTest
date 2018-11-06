#ifndef rpct_hwd_HardwareStorage_h
#define rpct_hwd_HardwareStorage_h

#include <string>
#include <vector>

#include "rpct/hwd/fwd.h"

#include "rpct/tools/Time.h"
#include "sqlite3.h"

#include "log4cplus/logger.h"

namespace rpct {
namespace hwd {

class HardwareStorage
{
protected:
    void prepareDataBaseStructure();
    void prepareStatements();
    void finalizeStatements();

    static log4cplus::Logger logger_;

public:
    enum Mode {
        read_  = SQLITE_OPEN_READONLY
        , write_ = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)
    };
public:
    // open file with sqlite
    // check the tables and create if something's missing
    HardwareStorage(std::string const & _filename
                    , Mode _mode = read_);

    ~HardwareStorage();

    void beginTransaction();
    void rollbackTransaction();
    void commitTransaction();

    std::string const & getFileName() const;

    DataType const & getDataType(std::string const & _name) const;
    DataType const & getDataType(integer_type _id) const;

    void registerSystem(System & _system
                        , DeviceType const & _devicetype);
    void registerSystem(System & _system
                        , DeviceType const & _devicetype
                        , DeviceConfiguration const & _properties);
    void loadSystem(System & _system
                    , rpct::tools::Time const & _time = rpct::tools::Time());

    DeviceType & registerDeviceType(System & _system
                                    , std::string const & _name);
    DeviceType & getDeviceType(System & _system
                               , std::string const & _name) const;
    DeviceType & getDeviceType(System & _system
                               , integer_type _id) const;
    void loadDeviceType(DeviceType & _devicetype) const;

    ParameterType & registerParameterType(System & _system
                                          , std::string const & _name
                                          , DataType const & _datatype);
    ParameterType & getParameterType(System & _system
                                     , std::string const & _name) const;
    ParameterType & getParameterType(System & _system
                                     , integer_type _id) const;

    Parameter const & registerParameter(DeviceType & _devicetype
                                        , ParameterType const & _parametertype
                                        , integer_type _position = 0
                                        , unsigned char _properties = 0x0);
    Parameter const & getParameter(DeviceType & _devicetype
                                   , ParameterType const & _parametertype
                                   , integer_type _position = 0) const;
    Parameter const & getParameter(DeviceType & _devicetype
                                   , std::string const & _parametertype_name
                                   , integer_type _position = 0) const;
    Parameter const & getParameter(DeviceType & _devicetype
                                   , integer_type _id) const;

    Service & registerService(System & _system
                              , std::string const & _name);
    Service & getService(System & _system
                         , std::string const & _name) const;
    Service & getService(System & _system
                         , integer_type _id) const;

    Connector const & registerConnector(DeviceType & _devicetype
                                        , Service const & _service
                                        , integer_type _position = 0
                                        , Direction _direction = bidirectional_);
    Connector const & getConnector(DeviceType & _devicetype
                                   , Service const & _service
                                   , integer_type _position = 0
                                   , Direction _direction = bidirectional_) const;
    Connector const & getConnector(DeviceType & _devicetype
                                   , integer_type _id) const;

    Configuration registerConfiguration(System & _system
                                        , std::string const & _name);
    Configuration getConfiguration(System & _system
                                   , std::string const & _name);
    void loadConfiguration(Configuration & _configuration
                           , rpct::tools::Time const & _time = rpct::tools::Time()) const;

    DeviceConfiguration & registerDeviceTypeConfiguration(Configuration & _configuration
                                                          , DeviceType const & _devicetype);
    DeviceConfiguration & getDeviceTypeConfiguration(Configuration & _configuration
                                                     , integer_type _id) const;

    StandaloneDeviceConfiguration registerDeviceTypeConfiguration(System & _system
                                                                  , DeviceType const & _devicetype);
    StandaloneDeviceConfiguration getDeviceTypeConfiguration(System & _system
                                                             , integer_type _id);

    void loadDeviceTypeConfiguration(DeviceConfiguration & _devicetypeconfiguration) const;

    void registerDeviceConfiguration(Configuration & _configuration
                                     , Device const & _device
                                     , DeviceConfiguration const & _devicetypeconfiguration);

    integer_type registerParameterSetting(integer_type _value);
    integer_type registerParameterSetting(float_type _value);
    integer_type getParameterSetting(float_type _value) const;
    integer_type registerParameterSetting(text_type const & _value);
    integer_type registerParameterSetting(blob_type const & _value);

    void registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                  , Parameter const & _parameter
                                  , integer_type _value);
    void registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                  , Parameter const & _parameter
                                  , float_type _value);
    void registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                  , Parameter const & _parameter
                                  , text_type const & _value);
    void registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                  , Parameter const & _parameter
                                  , blob_type const & _value);

    Connection registerConnection(Device & _src
                                  , Connector const & _srcconnector
                                  , Device & _dest
                                  , Connector const & _destconnector);
    Connection getConnection(Device & _src
                             , Connector const & _srcconnector
                             , Device & _dest
                             , Connector const & _destconnector) const;

    Device & registerDevice(System & _system
                            , DeviceType const & _devicetype
                            , DeviceConfiguration const & _properties);

    SnapshotType & registerSnapshotType(System & _system
                                        , std::string const & _name);
    SnapshotType & getSnapshotType(System & _system
                                   , std::string const & _name) const;
    SnapshotType & getSnapshotType(System & _system
                                   , integer_type _id) const;

    Snapshot takeSnapshot(System const & _system
                          , SnapshotType const & _snapshottype
                          , rpct::tools::Time const & _time = rpct::tools::Time());
    std::vector<Snapshot> getSnapshots(System const & _system
                                       , SnapshotType const & _snapshottype
                                       , rpct::tools::Time const & _start = rpct::tools::Time(0)
                                       , rpct::tools::Time const & _stop = rpct::tools::Time()) const;

    void loadSnapshotConfiguration(Configuration & _configuration
                                   , rpct::tools::Time const & _snapshot
                                   , rpct::tools::Time const & _previous = rpct::tools::Time(0)) const;
    void loadSnapshotObservables(Configuration & _configuration
                                 , rpct::tools::Time const & _snapshot
                                 , rpct::tools::Time const & _previous = rpct::tools::Time(0)) const;

    template<typename t_cpp_value>
    void publish(Device const & _device, Parameter const & _parameter, t_cpp_value const & _value
                 , rpct::tools::Time const & _time = rpct::tools::Time());
    void publish(Device const & _device, DeviceConfiguration const & _deviceconfiguration
                 , rpct::tools::Time const & _time = rpct::tools::Time());

    integer_type registerParameterValue(integer_type _value);
    integer_type registerParameterValue(float_type _value);
    integer_type getParameterValue(float_type _value) const;
    integer_type registerParameterValue(text_type const & _value);
    integer_type registerParameterValue(blob_type const & _value);

    void registerParameterValue(Device const & _device
                                , Parameter const & _parameter
                                , integer_type _value
                                , rpct::tools::Time const & _time = rpct::tools::Time());
    void registerParameterValue(Device const & _device
                                , Parameter const & _parameter
                                , float_type _value
                                , rpct::tools::Time const & _time = rpct::tools::Time());
    void registerParameterValue(Device const & _device
                                , Parameter const & _parameter
                                , text_type const & _value
                                , rpct::tools::Time const & _time = rpct::tools::Time());
    void registerParameterValue(Device const & _device
                                , Parameter const & _parameter
                                , blob_type const & _value
                                , rpct::tools::Time const & _time = rpct::tools::Time());

protected:
    void loadDeviceTypes(System & _system) const; // DeviceTypes, Parameters, Connectors
    void loadDevices(System & _system
                     , rpct::tools::Time const & _time = rpct::tools::Time()); // Devices, static Configurations

protected:
    std::string filename_;
    ::sqlite3 * db_;

    ::sqlite3_stmt * insert_system_, * insert_device_, * insert_systemdevice_;
    ::sqlite3_stmt * insert_connection_;

    ::sqlite3_stmt * insert_devicetype_, * insert_parametertype_
          , * insert_parameter_
          , * insert_service_, * insert_connector_;

    ::sqlite3_stmt * insert_configuration_, * insert_devicetypeconfiguration_, * insert_deviceconfiguration_;

    ::sqlite3_stmt * select_parametersettingfloat_, * select_parametersettingtext_, * select_parametersettingblob_;
    ::sqlite3_stmt * insert_parametersettingfloat_, * insert_parametersettingtext_, * insert_parametersettingblob_
          , * insert_parametersetting_;

    ::sqlite3_stmt * select_parametervaluefloat_, * select_parametervaluetext_, * select_parametervalueblob_;
    ::sqlite3_stmt * insert_parametervaluefloat_, * insert_parametervaluetext_, * insert_parametervalueblob_
          , * insert_parametervalue_;

    ::sqlite3_stmt * insert_deviceconfigurationvalue_;

    ::sqlite3_stmt * insert_snapshottype_, * insert_snapshot_, * select_snapshottype_, * select_snapshots_;

    ::sqlite3_stmt * load_devicetypeconfigurations_, * load_deviceconfigurations_;
    ::sqlite3_stmt * load_snapshot_devicetypeconfigurations_, * load_snapshot_deviceconfigurations_
          , * load_snapshot_deviceobservables_;

};

class Transaction
{
public:
    Transaction(HardwareStorage & _hwstorage, bool _begin = true);
    ~Transaction();

    void begin();
    void commit();
    void rollback();

protected:
    HardwareStorage & hwstorage_;
    bool open_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/HardwareStorage-inl.h"

#endif // rpct_hwd_HardwareStorage_h
