#include "rpct/hwd/HardwareStorage.h"

#include "log4cplus/loggingmacros.h"

#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <sstream>

#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/Device.h"
#include "rpct/hwd/System.h"

#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/Configuration.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/hwd/Connection.h"

#include "rpct/hwd/SnapshotType.h"
#include "rpct/hwd/Snapshot.h"

#include "rpct/tools/Murmur3a.h"
#include "rpct/tools/Date.h"

#include "sqlite3.h"

namespace rpct {
namespace hwd {

log4cplus::Logger HardwareStorage::logger_ = log4cplus::Logger::getInstance("HardwareStorage");

void HardwareStorage::prepareDataBaseStructure()
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    {
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT 1 FROM sqlite_master"
                                   " WHERE type='table' AND name='DeviceType'"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_step(_stmt)) != SQLITE_DONE)
            {
                ::sqlite3_finalize(_stmt);
                if ((_return & 0xff) == SQLITE_ROW)
                    return;
                throw std::runtime_error(std::string("Could not prepare the DataBase Structure")
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_finalize(_stmt);
    }

    ::sqlite3_exec(db_, "BEGIN TRANSACTION", 0, 0, 0);

    // Hardware
    ::sqlite3_exec(db_
                   , "CREATE TABLE DeviceType ("
                   "id INTEGER PRIMARY KEY"
                   ", name VARCHAR(255) NOT NULL"
                   ", UNIQUE (name)"
                   ");"
                   "CREATE UNIQUE INDEX DeviceType_name ON DeviceType (name);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE Service ("
                   "id INTEGER PRIMARY KEY"
                   ", name VARCHAR(255) NOT NULL"
                   ", UNIQUE (name)"
                   ");"
                   "CREATE UNIQUE INDEX Service_name ON Service (name);"
                   "INSERT INTO Service (id, name) VALUES (0, 'Host');"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE Connector ("
                   "id INTEGER PRIMARY KEY"
                   ", devicetype INTEGER NOT NULL"
                   ", service INTEGER DEFAULT 0"
                   ", position INTEGER DEFAULT 0"
                   ", direction INTEGER DEFAULT 2"
                   ", FOREIGN KEY (devicetype) REFERENCES DeviceType(id)"
                   ", FOREIGN KEY (service) REFERENCES Service(id)"
                   ", UNIQUE (devicetype, service, position, direction)"
                   ");"
                   "CREATE INDEX Connector_devicetype ON Connector (devicetype);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE DataType ("
                   "id INTEGER PRIMARY KEY"
                   ", name VARCHAR(255) NOT NULL"
                   ", UNIQUE (name)"
                   ");"
                   "CREATE UNIQUE INDEX DataType_name ON DataType (name);"
                   , 0, 0, 0);

    { // add the existing DataTypes
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO DataType (id, name)"
                                   " SELECT ?, ?"
                                   " UNION SELECT ?, ?"
                                   " UNION SELECT ?, ?"
                                   " UNION SELECT ?, ?"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, DataType::integer_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_text(_stmt, 2, DataType::integer_name_.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 3, DataType::float_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_text(_stmt, 4, DataType::float_name_.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 5, DataType::text_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_text(_stmt, 6, DataType::text_name_.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 7, DataType::blob_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_text(_stmt, 8, DataType::blob_name_.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
            || (_return = ::sqlite3_step(_stmt)) != SQLITE_DONE)
            {
                ::sqlite3_finalize(_stmt);
                ::sqlite3_exec(db_, "ROLLBACK TRANSACTION", 0, 0, 0);
                throw std::runtime_error(std::string("Could not add datatypes")
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_finalize(_stmt);
    }

    // assumes DataType::integer_id_ = 0 at the moment
    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterType ("
                   "id INTEGER PRIMARY KEY"
                   ", name VARCHAR(255) NOT NULL"
                   ", datatype INTEGER DEFAULT 0"
                   ", UNIQUE (name)"
                   ", FOREIGN KEY (datatype) REFERENCES DataType (id)"
                   ");"
                   "CREATE UNIQUE INDEX ParameterType_name ON ParameterType (name);"
                   "INSERT INTO ParameterType (id, name) VALUES (0, 'Default');"
                   , 0, 0, 0);

    { // add the default ParameterTypes
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterType (name, datatype)"
                                   " SELECT ?, ?"
                                   " UNION SELECT ?, ?"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_text(_stmt, 1, State::parametertype_name_.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 2, DataType::integer_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_text(_stmt, 3, DeviceFlags::parametertype_name_.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 4, DataType::integer_id_)) != SQLITE_OK
            || (_return = ::sqlite3_step(_stmt)) != SQLITE_DONE)
            {
                ::sqlite3_finalize(_stmt);
                ::sqlite3_exec(db_, "ROLLBACK TRANSACTION", 0, 0, 0);
                throw std::runtime_error(std::string("Could not add parametertypes")
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_finalize(_stmt);
    }

    ::sqlite3_exec(db_
                   , "CREATE TABLE Parameter ("
                   "id INTEGER PRIMARY KEY"
                   ", devicetype INTEGER NOT NULL"
                   ", parametertype INTEGER NOT NULL"
                   ", position INTEGER DEFAULT 0"
                   ", property BOOLEAN DEFAULT 0"
                   ", observable BOOLEAN DEFAULT 0"
                   ", configurable BOOLEAN DEFAULT 0"
                   ", FOREIGN KEY (devicetype) REFERENCES DeviceType(id)"
                   ", FOREIGN KEY (parametertype) REFERENCES ParameterType(id)"
                   ", UNIQUE (devicetype, parametertype, position)"
                   ");"
                   "CREATE INDEX Parameter_devicetype ON Parameter (devicetype);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE Device ("
                   "id INTEGER PRIMARY KEY"
                   ", devicetype INTEGER NOT NULL"
                   ", FOREIGN KEY (devicetype) REFERENCES DeviceType (id)"
                   ")"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE Connection ("
                   "id INTEGER PRIMARY KEY"
                   ", src INTEGER NOT NULL"
                   ", srcconnector INTEGER NOT NULL"
                   ", dest INTEGER NOT NULL"
                   ", destconnector INTEGER NOT NULL"
                   ", FOREIGN KEY (src) REFERENCES Device(id)"
                   ", FOREIGN KEY (srcconnector) REFERENCES Connector(id)"
                   ", FOREIGN KEY (dest) REFERENCES Device(id)"
                   ", FOREIGN KEY (destconnector) REFERENCES Connector(id)"
                   ");"
                   "CREATE INDEX Connection_src ON Connection (src);"
                   "CREATE INDEX Connection_dest ON Connection (dest);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE System ("
                   "id INTEGER PRIMARY KEY"
                   ", name VARCHAR(255) NOT NULL"
                   ", UNIQUE (name)"
                   ", FOREIGN KEY (id) REFERENCES Device(id)"
                   ");"
                   "CREATE UNIQUE INDEX System_name ON System (name);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE SystemDevice ("
                   "system INTEGER NOT NULL"
                   ", device INTEGER NOT NULL"
                   ", PRIMARY KEY (system, device)"
                   ", FOREIGN KEY (system) REFERENCES System (id)"
                   ", FOREIGN KEY (device) REFERENCES Device (id)"
                   ")"
                   , 0, 0, 0);

    // Configuration
    ::sqlite3_exec(db_
                   , "CREATE TABLE Configuration ("
                   "id INTEGER PRIMARY KEY"
                   ", name VARCHAR(255) NOT NULL"
                   ", UNIQUE (name)"
                   ");"
                   "CREATE UNIQUE INDEX Configuration_name ON Configuration (name);"
                   "INSERT INTO Configuration (id, name) VALUES (0, 'Properties');"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE DeviceTypeConfiguration ("
                   "id INTEGER PRIMARY KEY"
                   ", devicetype INTEGER NOT NULL"
                   ", FOREIGN KEY (devicetype) REFERENCES DeviceType (id)"
                   ")"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterSetting ("
                   "devicetypeconfiguration INTEGER NOT NULL"
                   ", parameter INTEGER NOT NULL"
                   ", setting INTEGER NOT NULL"
                   ", PRIMARY KEY (devicetypeconfiguration, parameter)"
                   ", FOREIGN KEY (devicetypeconfiguration) REFERENCES DeviceTypeConfiguration (id)"
                   ", FOREIGN KEY (parameter) REFERENCES Parameter (id)"
                   ")"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE DeviceConfiguration ("
                   "configuration INTEGER NOT NULL"
                   ", device INTEGER NOT NULL"
                   ", time INTEGER NOT NULL"
                   ", devicetypeconfiguration INTEGER NOT NULL"
                   ", PRIMARY KEY (configuration, device, time)"
                   ", FOREIGN KEY (configuration) REFERENCES Configuration (id)"
                   ", FOREIGN KEY (device) REFERENCES Device (id)"
                   ", FOREIGN KEY (devicetypeconfiguration) REFERENCES DeviceTypeConfiguration (id)"
                   ")"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterSettingFloat ("
                   "id INTEGER PRIMARY KEY"
                   ", value DOUBLE PRECISION NOT NULL"
                   ", UNIQUE (value)"
                   ");"
                   "CREATE UNIQUE INDEX ParameterSettingFloat_value ON ParameterSettingFloat (value);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterSettingText ("
                   "id INTEGER PRIMARY KEY"
                   ", value VARCHAR(255) NOT NULL"
                   ", hash INTEGER NOT NULL"
                   ");"
                   "CREATE INDEX ParameterSettingText_hash ON ParameterSettingText (hash);"
                   , 0, 0, 0);


    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterSettingBlob ("
                   "id INTEGER PRIMARY KEY"
                   ", value BLOB NOT NULL"
                   ", hash INTEGER NOT NULL"
                   ");"
                   "CREATE INDEX ParameterSettingBlob_hash ON ParameterSettingBlob (hash);"
                   , 0, 0, 0);

    // Measurements
    ::sqlite3_exec(db_
                   , "CREATE TABLE DeviceConfigurationValue ("
                   "device INTEGER NOT NULL"
                   ", time INTEGER NOT NULL"
                   ", devicetypeconfiguration INTEGER NOT NULL"
                   ", PRIMARY KEY (device, time)"
                   ", FOREIGN KEY (device) REFERENCES Device (id)"
                   ", FOREIGN KEY (devicetypeconfiguration) REFERENCES DeviceTypeConfiguration (id)"
                   ");"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE ConfigurationValue ("
                   "system INTEGER NOT NULL"
                   ", time INTEGER NOT NULL"
                   ", configuration INTEGER NOT NULL"
                   ", PRIMARY KEY (sytem, time)"
                   ", FOREIGN KEY (system) REFERENCES System (id)"
                   ", FOREIGN KEY (configuration) REFERENCES Configuration (id)"
                   ");"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterValue ("
                   "device INTEGER NOT NULL"
                   ", parameter INTEGER NOT NULL"
                   ", time INTEGER NOT NULL"
                   ", value INTEGER NOT NULL"
                   ", PRIMARY KEY (device, parameter, time)"
                   ", FOREIGN KEY (device) REFERENCES Device (id)"
                   ", FOREIGN KEY (parameter) REFERENCES Parameter (id)"
                   ");"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterValueFloat ("
                   "id INTEGER PRIMARY KEY"
                   ", value DOUBLE PRECISION NOT NULL"
                   ", UNIQUE (value)"
                   ");"
                   "CREATE UNIQUE INDEX ParameterValueFloat_value ON ParameterValueFloat (value);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterValueText ("
                   "id INTEGER PRIMARY KEY"
                   ", value VARCHAR(255) NOT NULL"
                   ", hash INTEGER NOT NULL"
                   ");"
                   "CREATE INDEX ParameterValueText_hash ON ParameterValueText (hash);"
                   , 0, 0, 0);


    ::sqlite3_exec(db_
                   , "CREATE TABLE ParameterValueBlob ("
                   "id INTEGER PRIMARY KEY"
                   ", value BLOB NOT NULL"
                   ", hash INTEGER NOT NULL"
                   ");"
                   "CREATE INDEX ParameterValueBlob_hash ON ParameterValueBlob (hash);"
                   , 0, 0, 0);

    // Snapshots
    ::sqlite3_exec(db_
                   , "CREATE TABLE SnapshotType ("
                   "id INTEGER PRIMARY KEY"
                   ", name VARCHAR(255) NOT NULL"
                   ", UNIQUE (name)"
                   ");"
                   "CREATE UNIQUE INDEX SnapshotType_name ON SnapshotType (name);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_
                   , "CREATE TABLE Snapshot ("
                   "id INTEGER PRIMARY KEY"
                   ", snapshottype INTEGER NOT NULL"
                   ", system INTEGER NOT NULL"
                   ", time INTEGER NOT NULL"
                   ", FOREIGN KEY (snapshottype) REFERENCES SnapshotType (id)"
                   ", FOREIGN KEY (system) REFERENCES System (id)"
                   ");"
                   "CREATE INDEX Snapshot_SnapshotType ON Snapshot (snapshottype);"
                   , 0, 0, 0);

    ::sqlite3_exec(db_, "COMMIT TRANSACTION", 0, 0, 0);
}

void HardwareStorage::prepareStatements()
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");

    int _return;
    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO System (id, name) VALUES (?, ?)"
                                   , -1, &insert_system_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_system_);
        throw std::runtime_error(std::string("Could not prepare Insert System statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO Device (devicetype) VALUES (?)"
                                   , -1, &insert_device_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_device_);
        throw std::runtime_error(std::string("Could not prepare Insert Device statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO SystemDevice (system, device)"
                                   " VALUES (?, ?)"
                                   , -1, &insert_systemdevice_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_systemdevice_);
        throw std::runtime_error(std::string("Could not prepare Insert System Device statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO Connection"
                                   " (src, srcconnector, dest, destconnector)"
                                   " VALUES (?, ?, ?, ?)"
                                   , -1, &insert_connection_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_connection_);
        throw std::runtime_error(std::string("Could not prepare Insert Connection statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO DeviceType (name) VALUES (?)"
                                   , -1, &insert_devicetype_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_devicetype_);
        throw std::runtime_error(std::string("Could not prepare Insert DeviceType statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterType (name, datatype) VALUES (?, ?)"
                                   , -1, &insert_parametertype_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametertype_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterType statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO Parameter (devicetype, parametertype, position, property, observable, configurable)"
                                   " VALUES (?, ?, ?, ?, ?, ?)"
                                   , -1, &insert_parameter_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parameter_);
        throw std::runtime_error(std::string("Could not prepare Insert Parameter statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO Service (name) VALUES (?)"
                                   , -1, &insert_service_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_service_);
        throw std::runtime_error(std::string("Could not prepare Insert Service statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO Connector (devicetype, service, position, direction)"
                                   " VALUES (?, ?, ?, ?)"
                                   , -1, &insert_connector_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_connector_);
        throw std::runtime_error(std::string("Could not prepare Insert Connector statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO Configuration (name) VALUES (?)"
                                   , -1, &insert_configuration_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_configuration_);
        throw std::runtime_error(std::string("Could not prepare Insert Configuration statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO DeviceTypeConfiguration (devicetype) VALUES (?)"
                                   , -1, &insert_devicetypeconfiguration_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_devicetypeconfiguration_);
        throw std::runtime_error(std::string("Could not prepare Insert DeviceTypeConfiguration statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO DeviceConfiguration"
                                   " (configuration, device, devicetypeconfiguration, time)"
                                   " VALUES (?, ?, ?, ?)"
                                   , -1, &insert_deviceconfiguration_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_deviceconfiguration_);
        throw std::runtime_error(std::string("Could not prepare Insert Device Configuartion statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT ParameterSettingFloat.id"
                                   " FROM ParameterSettingFloat"
                                   " WHERE ParameterSettingFloat.value=?"
                                   " LIMIT 1"
                                   , -1, &select_parametersettingfloat_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_parametersettingfloat_);
        throw std::runtime_error(std::string("Could not prepare Select ParameterSettingFloat statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT ParameterSettingText.id"
                                   ", ParameterSettingText.value"
                                   " FROM ParameterSettingText"
                                   " WHERE ParameterSettingText.hash=?"
                                   , -1, &select_parametersettingtext_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_parametersettingtext_);
        throw std::runtime_error(std::string("Could not prepare Select ParameterSettingText statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT ParameterSettingBlob.id"
                                   ", ParameterSettingBlob.value"
                                   " FROM ParameterSettingBlob"
                                   " WHERE ParameterSettingBlob.hash=?"
                                   , -1, &select_parametersettingblob_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_parametersettingblob_);
        throw std::runtime_error(std::string("Could not prepare Select ParameterSettingBlob statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterSettingFloat (value) VALUES (?)"
                                   , -1, &insert_parametersettingfloat_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametersettingfloat_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterSettingFloat statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterSettingText (value, hash) VALUES (?, ?)"
                                   , -1, &insert_parametersettingtext_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametersettingtext_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterSettingText statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterSettingBlob (value, hash) VALUES (?, ?)"
                                   , -1, &insert_parametersettingblob_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametersettingblob_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterSettingBlob statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterSetting"
                                   " (devicetypeconfiguration, parameter, setting)"
                                   " VALUES (?, ?, ?)"
                                   , -1, &insert_parametersetting_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametersetting_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterSetting statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT ParameterValueFloat.id"
                                   " FROM ParameterValueFloat"
                                   " WHERE ParameterValueFloat.value=?"
                                   " LIMIT 1"
                                   , -1, &select_parametervaluefloat_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_parametervaluefloat_);
        throw std::runtime_error(std::string("Could not prepare Select ParameterValueFloat statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT ParameterValueText.id"
                                   ", ParameterValueText.value"
                                   " FROM ParameterValueText"
                                   " WHERE ParameterValueText.hash=?"
                                   , -1, &select_parametervaluetext_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_parametervaluetext_);
        throw std::runtime_error(std::string("Could not prepare Select ParameterValueText statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT ParameterValueBlob.id"
                                   ", ParameterValueBlob.value"
                                   " FROM ParameterValueBlob"
                                   " WHERE ParameterValueBlob.hash=?"
                                   , -1, &select_parametervalueblob_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_parametervalueblob_);
        throw std::runtime_error(std::string("Could not prepare Select ParameterValueBlob statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterValueFloat (value) VALUES (?)"
                                   , -1, &insert_parametervaluefloat_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametervaluefloat_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterValueFloat statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterValueText (value, hash) VALUES (?, ?)"
                                   , -1, &insert_parametervaluetext_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametervaluetext_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterValueText statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterValueBlob (value, hash) VALUES (?, ?)"
                                   , -1, &insert_parametervalueblob_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametervalueblob_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterValueBlob statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO ParameterValue"
                                   " (device, parameter, time, value)"
                                   " VALUES (?, ?, ?, ?)"
                                   , -1, &insert_parametervalue_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_parametervalue_);
        throw std::runtime_error(std::string("Could not prepare Insert ParameterValue statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO DeviceConfigurationValue (device, devicetypeconfiguration, time)"
                                   " VALUES (?, ?, ?)"
                                   , -1, &insert_deviceconfigurationvalue_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_deviceconfigurationvalue_);
        throw std::runtime_error(std::string("Could not prepare Insert DeviceConfigurationValue statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO SnapshotType (name) VALUES (?)"
                                   , -1, &insert_snapshottype_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_snapshottype_);
        throw std::runtime_error(std::string("Could not prepare Insert SnapshotType statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "INSERT INTO Snapshot (system, snapshottype, time)"
                                   " VALUES (?, ?, ?)"
                                   , -1, &insert_snapshot_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(insert_snapshot_);
        throw std::runtime_error(std::string("Could not prepare Insert Snapshot statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT SnapshotType.id FROM SnapshotType"
                                   " WHERE SnapshotType.name=?"
                                   " LIMIT 1"
                                   , -1, &select_snapshottype_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_snapshottype_);
        throw std::runtime_error(std::string("Could not prepare Select SnapshotType statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return = ::sqlite3_prepare_v2(db_
                                   , "SELECT id, time"
                                   " FROM Snapshot"
                                   " WHERE snapshottype=? AND system=? AND time BETWEEN ? AND ?"
                                   , -1, &select_snapshots_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(select_snapshots_);
        throw std::runtime_error(std::string("Could not prepare Select Snapshots statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceConfiguration.devicetypeconfiguration"
                               ", DeviceType.id, DeviceType.name"
                               ", Parameter.id"
                               ", ParameterType.datatype"
                               ", ParameterSetting.setting"
                               ", ParameterSettingFloat.value, ParameterSettingText.value, ParameterSettingBlob.value"
                               " FROM (SELECT SystemDevice.device as device"
                               "  , MAX(DeviceConfiguration.time) as time"
                               "   FROM SystemDevice"
                               "   INNER JOIN DeviceConfiguration ON (SystemDevice.device=DeviceConfiguration.device)"
                               "   WHERE SystemDevice.system=?"
                               "   AND DeviceConfiguration.configuration=?"
                               "   AND DeviceConfiguration.time<=?"
                               "   GROUP BY DeviceConfiguration.device) MaxTime"
                               " INNER JOIN Device ON (MaxTime.device=Device.id)"
                               " INNER JOIN DeviceType ON (Device.devicetype=DeviceType.id)"
                               " INNER JOIN DeviceConfiguration"
                               "   ON (DeviceConfiguration.configuration=? AND MaxTime.device=DeviceConfiguration.device AND MaxTime.time=DeviceConfiguration.time)"
                               " LEFT OUTER JOIN ParameterSetting"
                               "   ON (DeviceConfiguration.devicetypeconfiguration=ParameterSetting.devicetypeconfiguration)"
                               " LEFT OUTER JOIN Parameter ON (ParameterSetting.parameter=Parameter.id)"
                               " LEFT OUTER JOIN ParameterType ON (Parameter.parametertype=ParameterType.id)"
                               " LEFT OUTER JOIN ParameterSettingFloat"
                               "   ON (ParameterSetting.setting=ParameterSettingFloat.id AND ParameterType.datatype=?)"
                               " LEFT OUTER JOIN ParameterSettingText"
                               "   ON (ParameterSetting.setting=ParameterSettingText.id AND ParameterType.datatype=?)"
                               " LEFT OUTER JOIN ParameterSettingBlob"
                               "   ON (ParameterSetting.setting=ParameterSettingBlob.id AND ParameterType.datatype=?)"
                               " ORDER BY DeviceType.id, DeviceConfiguration.devicetypeconfiguration"
                               , -1, &load_devicetypeconfigurations_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(load_devicetypeconfigurations_);
        throw std::runtime_error(std::string("Could not prepare Load DeviceTypeConfigurations statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceConfiguration.device, DeviceConfiguration.devicetypeconfiguration"
                               " FROM (SELECT SystemDevice.device as device"
                               "  , MAX(DeviceConfiguration.time) as time"
                               "   FROM SystemDevice"
                               "   INNER JOIN DeviceConfiguration ON (SystemDevice.device=DeviceConfiguration.device)"
                               "   WHERE SystemDevice.system=?"
                               "   AND DeviceConfiguration.configuration=?"
                               "   AND DeviceConfiguration.time<=?"
                               "   GROUP BY DeviceConfiguration.device) MaxTime"
                               " INNER JOIN DeviceConfiguration"
                               "   ON (DeviceConfiguration.configuration=? AND MaxTime.device=DeviceConfiguration.device AND MaxTime.time=DeviceConfiguration.time)"
                               , -1, &load_deviceconfigurations_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(load_deviceconfigurations_);
        throw std::runtime_error(std::string("Could not prepare Load DeviceConfigurations statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceConfigurationValue.devicetypeconfiguration"
                               ", DeviceType.id, DeviceType.name"
                               ", Parameter.id"
                               ", ParameterType.datatype"
                               ", ParameterSetting.setting"
                               ", ParameterSettingFloat.value, ParameterSettingText.value, ParameterSettingBlob.value"
                               " FROM (SELECT SystemDevice.device as device"
                               "  , MAX(DeviceConfigurationValue.time) as time"
                               "   FROM SystemDevice"
                               "   INNER JOIN DeviceConfigurationValue ON (SystemDevice.device=DeviceConfigurationValue.device)"
                               "   WHERE SystemDevice.system=?"
                               "   AND DeviceConfigurationValue.time BETWEEN ? and ?"
                               "   GROUP BY DeviceConfigurationValue.device) MaxTime"
                               " INNER JOIN Device ON (MaxTime.device=Device.id)"
                               " INNER JOIN DeviceType ON (Device.devicetype=DeviceType.id)"
                               " INNER JOIN DeviceConfigurationValue"
                               "   ON (MaxTime.device=DeviceConfigurationValue.device AND MaxTime.time=DeviceConfigurationValue.time)"
                               " LEFT OUTER JOIN ParameterSetting"
                               "   ON (DeviceConfigurationValue.devicetypeconfiguration=ParameterSetting.devicetypeconfiguration)"
                               " LEFT OUTER JOIN Parameter ON (ParameterSetting.parameter=Parameter.id)"
                               " LEFT OUTER JOIN ParameterType ON (Parameter.parametertype=ParameterType.id)"
                               " LEFT OUTER JOIN ParameterSettingFloat"
                               "   ON (ParameterSetting.setting=ParameterSettingFloat.id AND ParameterType.datatype=?)"
                               " LEFT OUTER JOIN ParameterSettingText"
                               "   ON (ParameterSetting.setting=ParameterSettingText.id AND ParameterType.datatype=?)"
                               " LEFT OUTER JOIN ParameterSettingBlob"
                               "   ON (ParameterSetting.setting=ParameterSettingBlob.id AND ParameterType.datatype=?)"
                               " ORDER BY DeviceType.id, DeviceConfigurationValue.devicetypeconfiguration"
                               , -1, &load_snapshot_devicetypeconfigurations_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(load_snapshot_devicetypeconfigurations_);
        throw std::runtime_error(std::string("Could not prepare Load Snapshot DeviceTypeConfigurations statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceConfigurationValue.device, DeviceConfigurationValue.devicetypeconfiguration"
                               " FROM (SELECT SystemDevice.device as device"
                               "  , MAX(DeviceConfigurationValue.time) as time"
                               "   FROM SystemDevice"
                               "   INNER JOIN DeviceConfigurationValue ON (SystemDevice.device=DeviceConfigurationValue.device)"
                               "   WHERE SystemDevice.system=?"
                               "   AND DeviceConfigurationValue.time BETWEEN ? AND ?"
                               "   GROUP BY DeviceConfigurationValue.device) MaxTime"
                               " INNER JOIN DeviceConfigurationValue"
                               "   ON (MaxTime.device=DeviceConfigurationValue.device AND MaxTime.time=DeviceConfigurationValue.time)"
                               , -1, &load_snapshot_deviceconfigurations_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(load_snapshot_deviceconfigurations_);
        throw std::runtime_error(std::string("Could not prepare Load Snapshot DeviceConfigurations statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

    _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT ParameterValue.device"
                               ", DeviceType.id, DeviceType.name"
                               ", Parameter.id"
                               ", ParameterType.datatype"
                               ", ParameterValue.value"
                               ", ParameterValueFloat.value, ParameterValueText.value, ParameterValueBlob.value"
                               " FROM (SELECT SystemDevice.device as device, ParameterValue.parameter as parameter"
                               "  , MAX(ParameterValue.time) as time"
                               "   FROM SystemDevice"
                               "   INNER JOIN ParameterValue ON (SystemDevice.device=ParameterValue.device)"
                               "   WHERE SystemDevice.system=?"
                               "   AND ParameterValue.time BETWEEN ? AND ?"
                               "   GROUP BY SystemDevice.device, ParameterValue.parameter) MaxTime"
                               " INNER JOIN ParameterValue"
                               "   ON (MaxTime.device=ParameterValue.device AND MaxTime.parameter=ParameterValue.parameter AND MaxTime.time=ParameterValue.time)"
                               " INNER JOIN Parameter ON (ParameterValue.parameter=Parameter.id)"
                               " INNER JOIN ParameterType ON (Parameter.parametertype=ParameterType.id)"
                               " INNER JOIN DeviceType ON (Parameter.devicetype=DeviceType.id)"
                               " LEFT OUTER JOIN ParameterValueFloat"
                               "   ON (ParameterValue.value=ParameterValueFloat.id AND ParameterType.datatype=?)"
                               " LEFT OUTER JOIN ParameterValueText"
                               "   ON (ParameterValue.value=ParameterValueText.id AND ParameterType.datatype=?)"
                               " LEFT OUTER JOIN ParameterValueBlob"
                               "   ON (ParameterValue.value=ParameterValueBlob.id AND ParameterType.datatype=?)"
                               " ORDER BY DeviceType.id, ParameterValue.device"
                               , -1, &load_snapshot_deviceobservables_, 0);
    if (_return != SQLITE_OK) {
        ::sqlite3_finalize(load_snapshot_deviceobservables_);
        throw std::runtime_error(std::string("Could not prepare Load Snapshot DeviceObservables statement: ")
                                 + std::string(::sqlite3_errmsg(db_)));
    }

}

void HardwareStorage::finalizeStatements()
{
    ::sqlite3_finalize(insert_system_);
    ::sqlite3_finalize(insert_device_);
    ::sqlite3_finalize(insert_systemdevice_);
    ::sqlite3_finalize(insert_connection_);

    ::sqlite3_finalize(insert_devicetype_);
    ::sqlite3_finalize(insert_parametertype_);
    ::sqlite3_finalize(insert_parameter_);
    ::sqlite3_finalize(insert_service_);
    ::sqlite3_finalize(insert_connector_);

    ::sqlite3_finalize(insert_configuration_);
    ::sqlite3_finalize(insert_devicetypeconfiguration_);
    ::sqlite3_finalize(insert_deviceconfiguration_);

    ::sqlite3_finalize(select_parametersettingfloat_);
    ::sqlite3_finalize(select_parametersettingtext_);
    ::sqlite3_finalize(select_parametersettingblob_);
    ::sqlite3_finalize(insert_parametersettingfloat_);
    ::sqlite3_finalize(insert_parametersettingtext_);
    ::sqlite3_finalize(insert_parametersettingblob_);
    ::sqlite3_finalize(insert_parametersetting_);

    ::sqlite3_finalize(select_parametervaluefloat_);
    ::sqlite3_finalize(select_parametervaluetext_);
    ::sqlite3_finalize(select_parametervalueblob_);
    ::sqlite3_finalize(insert_parametervaluefloat_);
    ::sqlite3_finalize(insert_parametervaluetext_);
    ::sqlite3_finalize(insert_parametervalueblob_);
    ::sqlite3_finalize(insert_parametervalue_);

    ::sqlite3_finalize(insert_deviceconfigurationvalue_);

    ::sqlite3_finalize(insert_snapshottype_);
    ::sqlite3_finalize(insert_snapshot_);
    ::sqlite3_finalize(select_snapshottype_);
    ::sqlite3_finalize(select_snapshots_);

    ::sqlite3_finalize(load_devicetypeconfigurations_);
    ::sqlite3_finalize(load_deviceconfigurations_);
    ::sqlite3_finalize(load_snapshot_devicetypeconfigurations_);
    ::sqlite3_finalize(load_snapshot_deviceconfigurations_);
    ::sqlite3_finalize(load_snapshot_deviceobservables_);
}

HardwareStorage::HardwareStorage(std::string const & _filename
                                 , Mode _mode)
    : filename_(_filename)
    , db_(0)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    if (::sqlite3_open_v2(filename_.c_str(), &db_, _mode, 0) != SQLITE_OK)
        throw std::runtime_error(std::string("Could not open SQLite file ")
                                 + filename_
                                 + " to create HardwareStorage: "
                                 + std::string(::sqlite3_errmsg(db_)));

    ::sqlite3_extended_result_codes(db_, 1);

    prepareDataBaseStructure();
    prepareStatements();
}

HardwareStorage::~HardwareStorage()
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    finalizeStatements();
    ::sqlite3_close(db_);
}

void HardwareStorage::beginTransaction()
{
    ::sqlite3_exec(db_, "BEGIN TRANSACTION", 0, 0, 0);
}

void HardwareStorage::commitTransaction()
{
    ::sqlite3_exec(db_, "COMMIT TRANSACTION", 0, 0, 0);
}

void HardwareStorage::rollbackTransaction()
{
    ::sqlite3_exec(db_, "ROLLBACK TRANSACTION", 0, 0, 0);
}

void HardwareStorage::registerSystem(System & _system
                                     , DeviceType const & _devicetype)
{
    registerSystem(_system, _devicetype, DeviceConfiguration::default_);
}

void HardwareStorage::registerSystem(System & _system
                                     , DeviceType const & _devicetype
                                     , DeviceConfiguration const & _properties)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTSYSTEM", 0, 0, 0);

    int _return;
    { // The Device
        if ((_return = ::sqlite3_bind_int64(insert_device_, 1, _devicetype.getId())) != SQLITE_OK
            || (_return = ::sqlite3_step(insert_device_)) != SQLITE_DONE)
            {
                ::sqlite3_reset(insert_device_);
                ::sqlite3_exec(db_, "ROLLBACK TO INSERTSYSTEM", 0, 0, 0);
                if ((_return & 0xff) == SQLITE_READONLY)
                    return loadSystem(_system);
                else
                    throw std::runtime_error(std::string("Could not register System ")
                                             + _system.getName()
                                             + " with DeviceType " + _devicetype.getName()
                                             + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_reset(insert_device_);
    }

    integer_type _id = ::sqlite3_last_insert_rowid(db_);

    { // The System
        if ((_return = ::sqlite3_bind_int64(insert_system_, 1, _id)) != SQLITE_OK
            || (_return = ::sqlite3_bind_text(insert_system_, 2, _system.getName().c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
            || (_return = ::sqlite3_step(insert_system_)) != SQLITE_DONE)
            {
                ::sqlite3_reset(insert_system_);
                ::sqlite3_exec(db_, "ROLLBACK TO INSERTSYSTEM", 0, 0, 0);
                if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                    return loadSystem(_system);
                else
                    throw std::runtime_error(std::string("Could not register System ")
                                             + _system.getName()
                                             + " with DeviceType " + _devicetype.getName()
                                             + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_reset(insert_system_);
    }

    { // The System link
        if ((_return = ::sqlite3_bind_int64(insert_systemdevice_, 1, _id)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(insert_systemdevice_, 2, _id)) != SQLITE_OK
            || (_return = ::sqlite3_step(insert_systemdevice_)) != SQLITE_DONE)
            {
                ::sqlite3_reset(insert_systemdevice_);
                ::sqlite3_exec(db_, "ROLLBACK TO INSERTSYSTEM", 0, 0, 0);
                throw std::runtime_error(std::string("Could not register System ")
                                         + _system.getName()
                                         + " with DeviceType " + _devicetype.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_reset(insert_systemdevice_);
    }

    if (_properties.getId() != -1)
        { // The Properties link
            rpct::tools::Time _time;
            if ((_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 1, 0)) != SQLITE_OK
                || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 2, _id)) != SQLITE_OK
                || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 3, _properties.getId())) != SQLITE_OK
                || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 4, _time.ustime())) != SQLITE_OK
                || (_return = ::sqlite3_step(insert_deviceconfiguration_)) != SQLITE_DONE)
                {
                    ::sqlite3_reset(insert_deviceconfiguration_);
                    ::sqlite3_exec(db_, "ROLLBACK TO INSERTSYSTEM", 0, 0, 0);
                    throw std::runtime_error(std::string("Could not register System ")
                                             + _system.getName()
                                             + " with DeviceType " + _devicetype.getName()
                                             + ": " + std::string(::sqlite3_errmsg(db_)));
                }
            ::sqlite3_reset(insert_deviceconfiguration_);
        }

    ::sqlite3_exec(db_, "RELEASE INSERTSYSTEM", 0, 0, 0);

    _system.setId(_id);
    _system.setDeviceType(_devicetype);
    _system.setProperties(_properties);
}

void HardwareStorage::loadSystem(System & _system, rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    // get System Device, DeviceType, Parameters and Connectors
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT Device.id, Device.devicetype"
                               " FROM System"
                               " INNER JOIN Device ON (System.id=Device.id)"
                               " WHERE System.name=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);

    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_text(_stmt, 1, _system.getName().c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            throw std::runtime_error(std::string("Could not get System ")
                                     + _system.getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }

    _system.setId(::sqlite3_column_int64(_stmt, 0));
    _system.setDeviceType(getDeviceType(_system, ::sqlite3_column_int64(_stmt, 1)));

    ::sqlite3_finalize(_stmt);

    // DeviceConfiguration for System, if any, is loaded within loadDevices
    loadDeviceTypes(_system);
    loadDevices(_system, _time);
}

void HardwareStorage::loadDeviceTypes(System & _system) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    { // DeviceTypes and Parameters
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT DeviceType.id, DeviceType.name"
                                   ", ParameterType.id, ParameterType.name, ParameterType.datatype"
                                   ", Parameter.id, Parameter.position"
                                   ", Parameter.property, Parameter.observable, Parameter.configurable"
                                   " FROM SystemDevice"
                                   " INNER JOIN Device ON (SystemDevice.device = Device.id)"
                                   " INNER JOIN DeviceType ON (Device.devicetype = DeviceType.id)"
                                   " LEFT OUTER JOIN Parameter ON (DeviceType.id = Parameter.devicetype)"
                                   " LEFT OUTER JOIN ParameterType ON (Parameter.parametertype = ParameterType.id)"
                                   " WHERE SystemDevice.system=?"
                                   " ORDER BY DeviceType.id"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, _system.getId())) != SQLITE_OK)
            {
                ::sqlite3_finalize(_stmt);
                throw std::runtime_error(std::string("Could not load System ")
                                         + _system.getName() + " DeviceTypes and Parameters: "
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _devicetype_id;
        std::string _devicetype_name;
        integer_type _parametertype_id, _datatype;
        std::string _parametertype_name;
        integer_type _parameter_id;
        integer_type _parameter_position;
        bool _parameter_property, _parameter_observable, _parameter_configurable;

        DeviceType * _devicetype(0);

        while ((_return = ::sqlite3_step(_stmt)) == SQLITE_ROW)
            {
                _devicetype_id = ::sqlite3_column_int64(_stmt, 0);
                if (!_devicetype || _devicetype->getId() != _devicetype_id)
                    {
                        _devicetype_name = (char const *)(::sqlite3_column_text(_stmt, 1));
                        _devicetype
                            = &(_system.addDeviceType(_devicetype_id, _devicetype_name));
                    }
                if (::sqlite3_column_type(_stmt, 2) != SQLITE_NULL)
                    {
                        _parametertype_id = ::sqlite3_column_int64(_stmt, 2);
                        _parametertype_name = (char const *)(::sqlite3_column_text(_stmt, 3));
                        _datatype = ::sqlite3_column_int64(_stmt, 4);

                        _parameter_id = ::sqlite3_column_int64(_stmt, 5);
                        _parameter_position = ::sqlite3_column_int64(_stmt, 6);
                        _parameter_property = ::sqlite3_column_int(_stmt, 7);
                        _parameter_observable = ::sqlite3_column_int(_stmt, 8);
                        _parameter_configurable = ::sqlite3_column_int(_stmt, 9);

                        unsigned char _parameter_properties
                            = (_parameter_property ? Parameter::is_property_ : 0x0)
                            | (_parameter_observable ? Parameter::is_observable_ : 0x0)
                            | (_parameter_configurable ? Parameter::is_configurable_ : 0x0);

                        ParameterType const & _parametertype
                            = _system.addParameterType(_parametertype_id
                                                       , _parametertype_name
                                                       , getDataType(_datatype));
                        _devicetype->addParameter(_parameter_id
                                                  , _parametertype
                                                  , _parameter_position
                                                  , _parameter_properties);
                    }
            }
        ::sqlite3_finalize(_stmt);
    }

    { // DeviceType Connectors
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT DeviceType.id, DeviceType.name"
                                   ", Service.id, Service.name"
                                   ", Connector.id, Connector.position, Connector.direction"
                                   " FROM SystemDevice"
                                   " INNER JOIN Device ON (SystemDevice.device = Device.id)"
                                   " INNER JOIN DeviceType ON (Device.devicetype = DeviceType.id)"
                                   " INNER JOIN Connector ON (DeviceType.id = Connector.devicetype)"
                                   " INNER JOIN Service ON (Connector.service = Service.id)"
                                   " WHERE SystemDevice.system=?"
                                   " ORDER BY DeviceType.id"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, _system.getId())) != SQLITE_OK)
            {
                ::sqlite3_finalize(_stmt);
                throw std::runtime_error(std::string("Could not load System ")
                                         + _system.getName() + " Connectors: "
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _devicetype_id;
        std::string _devicetype_name;
        integer_type _service_id;
        std::string _service_name;
        integer_type _connector_id, _connector_position;
        int _connector_direction;

        DeviceType * _devicetype(0);
        Service const * _service(0);

        while ((_return = ::sqlite3_step(_stmt)) == SQLITE_ROW)
            {
                _devicetype_id = ::sqlite3_column_int64(_stmt, 0);
                if (!_devicetype || _devicetype->getId() != _devicetype_id)
                    {
                        _devicetype_name = (char const *)(::sqlite3_column_text(_stmt, 1));
                        _devicetype
                            = &(_system.getDeviceType(_devicetype_name));
                    }
                _service_id = ::sqlite3_column_int64(_stmt, 2);
                if (!_service || _service->getId() != _service_id)
                    {
                        _service_name = (char const *)(::sqlite3_column_text(_stmt, 3));
                        _service
                            = &(_system.addService(_service_id, _service_name));
                    }
                _connector_id = ::sqlite3_column_int64(_stmt, 4);
                _connector_position = ::sqlite3_column_int64(_stmt, 5);
                _connector_direction = ::sqlite3_column_int(_stmt, 6);
                _devicetype->addConnector(_connector_id
                                          , *_service
                                          , _connector_position
                                          , (Direction)(_connector_direction));
            }
        ::sqlite3_finalize(_stmt);
    }
}

void HardwareStorage::loadDevices(System & _system, rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    Configuration _configuration = getConfiguration(_system, "Properties");
    loadConfiguration(_configuration, _time);

    if (_configuration.hasDeviceConfiguration(_system))
        _system.setProperties(_configuration.getDeviceConfiguration(_system.getId()));

    { // devices
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT Device.id"
                                   ", DeviceType.id, DeviceType.name"
                                   " FROM SystemDevice"
                                   " INNER JOIN Device ON (SystemDevice.device = Device.id)"
                                   " INNER JOIN DeviceType on Device.devicetype=DeviceType.id"
                                   " WHERE SystemDevice.system=?"
                                   " ORDER BY DeviceType.id"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, _system.getId())) != SQLITE_OK)
            {
                ::sqlite3_finalize(_stmt);
                throw std::runtime_error(std::string("Could not load System ")
                                         + _system.getName() + " Devices: "
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _device_id, _devicetype_id;
        std::string _devicetype_name;

        DeviceType const * _devicetype(0);

        while ((_return = ::sqlite3_step(_stmt)) == SQLITE_ROW)
            {
                _device_id = ::sqlite3_column_int64(_stmt, 0);
                _devicetype_id = ::sqlite3_column_int64(_stmt, 1);
                if (!_devicetype || _devicetype->getId() != _devicetype_id)
                    {
                        _devicetype_name = (char const *)(::sqlite3_column_text(_stmt, 2));
                        _devicetype = &(_system.getDeviceType(_devicetype_name));
                    }
                _system.addDevice(_device_id
                                  , *_devicetype
                                  , _configuration.getDeviceConfiguration(_device_id));
            }
        ::sqlite3_finalize(_stmt);
    }
    { // Connections
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT Connection.id, Connection.src, Connection.srcconnector, Connection.dest, Connection.destconnector"
                                   " FROM Connection"
                                   " INNER JOIN SystemDevice ON (Connection.src=SystemDevice.device)"
                                   " WHERE SystemDevice.system=?"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, _system.getId())) != SQLITE_OK)
            {
                ::sqlite3_finalize(_stmt);
                throw std::runtime_error(std::string("Could not load System ")
                                         + _system.getName() + " Connections: "
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _connection_id
            , _src_id
            , _srcconnector_id
            , _dest_id
            , _destconnector_id;

        while ((_return = ::sqlite3_step(_stmt)) == SQLITE_ROW)
            {
                _connection_id = ::sqlite3_column_int64(_stmt, 0);
                _src_id = ::sqlite3_column_int64(_stmt, 1);
                _srcconnector_id = ::sqlite3_column_int64(_stmt, 2);
                _dest_id = ::sqlite3_column_int64(_stmt, 3);
                _destconnector_id = ::sqlite3_column_int64(_stmt, 4);

                Device & _src = _system.getDevice(_src_id);
                Connector const & _srcconnector
                    = _src.getDeviceType().getConnector(_srcconnector_id);
                Device & _dest = _system.getDevice(_dest_id);
                Connector const & _destconnector
                    = _dest.getDeviceType().getConnector(_destconnector_id);

                Connection _connection(_connection_id
                                       , _src, _srcconnector, _dest, _destconnector);
                _src.addConnection(_connection);
                _dest.addConnection(_connection);
            }
        ::sqlite3_finalize(_stmt);
    }
}

DeviceType & HardwareStorage::registerDeviceType(System & _system
                                                 , std::string const & _name)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_text(insert_devicetype_, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_devicetype_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_devicetype_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getDeviceType(_system, _name);
            else
                throw std::runtime_error(std::string("Could not register DeviceType ")
                                         + _name
                                         + " for System " + _system.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_devicetype_);

    return _system.addDeviceType(::sqlite3_last_insert_rowid(db_), _name);
}

DeviceType & HardwareStorage::getDeviceType(System & _system
                                            , std::string const & _name) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceType.id"
                               " FROM DeviceType"
                               " WHERE DeviceType.name=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_text(_stmt, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            throw std::runtime_error(std::string("Could not get DeviceType ")
                                     + _name
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    integer_type _id = ::sqlite3_column_int64(_stmt, 0);
    ::sqlite3_finalize(_stmt);

    DeviceType & _devicetype = _system.addDeviceType(_id, _name);
    loadDeviceType(_devicetype);
    return _devicetype;
}

DeviceType & HardwareStorage::getDeviceType(System & _system
                                            , integer_type _id) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceType.name"
                               " FROM DeviceType"
                               " WHERE DeviceType.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get DeviceType " << _id
                      << ": " << std::string(::sqlite3_errmsg(db_));
            throw std::runtime_error(_errormsg.str());
        }
    std::string _name = (char const *)(::sqlite3_column_text(_stmt, 0));
    ::sqlite3_finalize(_stmt);

    DeviceType & _devicetype = _system.addDeviceType(_id, _name);
    loadDeviceType(_devicetype);
    return _devicetype;
}

void HardwareStorage::loadDeviceType(DeviceType & _devicetype) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    System & _system(_devicetype.getSystem());

    { // Parameters
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT ParameterType.id, ParameterType.name, ParameterType.datatype"
                                   ", Parameter.id, Parameter.position"
                                   ", Parameter.property, Parameter.observable, Parameter.configurable"
                                   " FROM Parameter"
                                   " INNER JOIN ParameterType ON (Parameter.parametertype = ParameterType.id)"
                                   " WHERE Parameter.devicetype=?"
                                   " ORDER BY ParameterType.id"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, _devicetype.getId())) != SQLITE_OK)
            {
                ::sqlite3_finalize(_stmt);
                throw std::runtime_error(std::string("Could not get DeviceType ")
                                         + _devicetype.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _parametertype_id, _datatype_id;
        std::string _parametertype_name;
        integer_type _parameter_id;
        integer_type _parameter_position;
        bool _parameter_property, _parameter_observable, _parameter_configurable;

        ParameterType const * _parametertype(0);

        while ((_return = ::sqlite3_step(_stmt)) == SQLITE_ROW)
            {
                _parametertype_id = ::sqlite3_column_int64(_stmt, 0);
                if (!_parametertype || _parametertype->getId() != _parametertype_id)
                    {
                        _parametertype_name = (char const *)(::sqlite3_column_text(_stmt, 1));
                        _datatype_id = ::sqlite3_column_int64(_stmt, 2);
                        _parametertype = &(_system.addParameterType(_parametertype_id
                                                                    , _parametertype_name
                                                                    , getDataType(_datatype_id)));
                    }
                _parameter_id = ::sqlite3_column_int64(_stmt, 3);
                _parameter_position = ::sqlite3_column_int64(_stmt, 4);
                _parameter_property = ::sqlite3_column_int(_stmt, 5);
                _parameter_observable = ::sqlite3_column_int(_stmt, 6);
                _parameter_configurable = ::sqlite3_column_int(_stmt, 7);

                unsigned char _parameter_properties
                    = (_parameter_property ? Parameter::is_property_ : 0x0)
                    | (_parameter_observable ? Parameter::is_observable_ : 0x0)
                    | (_parameter_configurable ? Parameter::is_configurable_ : 0x0);

                _devicetype.addParameter(_parameter_id
                                         , *_parametertype
                                         , _parameter_position
                                         , _parameter_properties);
            }
        ::sqlite3_finalize(_stmt);
    }

    { // Connectors
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT Service.id, Service.name"
                                   ", Connector.id, Connector.position, Connector.direction"
                                   " FROM Connector"
                                   " INNER JOIN Service ON (Connector.service = Service.id)"
                                   " WHERE Connector.devicetype=?"
                                   " ORDER BY Service.id"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, _devicetype.getId())) != SQLITE_OK)
            {
                ::sqlite3_finalize(_stmt);
                throw std::runtime_error(std::string("Could not get DeviceType Connectors")
                                         + _devicetype.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _service_id;
        std::string _service_name;
        integer_type _connector_id, _connector_position;
        int _connector_direction;

        Service const * _service(0);

        while ((_return = ::sqlite3_step(_stmt)) == SQLITE_ROW)
            {
                _service_id = ::sqlite3_column_int64(_stmt, 0);
                if (!_service || _service->getId() != _service_id)
                    {
                        _service_name = (char const *)(::sqlite3_column_text(_stmt, 1));
                        _service = &(_system.addService(_service_id, _service_name));
                    }

                _connector_id = ::sqlite3_column_int64(_stmt, 2);
                _connector_position = ::sqlite3_column_int64(_stmt, 3);
                _connector_direction = ::sqlite3_column_int(_stmt, 4);
                _devicetype.addConnector(_connector_id
                                         , *_service
                                         , _connector_position
                                         , (Direction)(_connector_direction));
            }
        ::sqlite3_finalize(_stmt);
    }
}

ParameterType & HardwareStorage::registerParameterType(System & _system
                                                       , std::string const & _name
                                                       , DataType const & _datatype)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_text(insert_parametertype_, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametertype_, 2, _datatype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametertype_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametertype_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getParameterType(_system, _name);
            else
                throw std::runtime_error(std::string("Could not register ParameterType ")
                                         + _name
                                         + " for System " + _system.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametertype_);

    return _system.addParameterType(::sqlite3_last_insert_rowid(db_)
                                    , _name
                                    , _datatype);
}

ParameterType & HardwareStorage::getParameterType(System & _system
                                                  , std::string const & _name) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT ParameterType.id, ParameterType.datatype"
                               " FROM ParameterType"
                               " WHERE ParameterType.name=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_text(_stmt, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            throw std::runtime_error(std::string("Could not get ParameterType ")
                                     + _name
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    integer_type _id = ::sqlite3_column_int64(_stmt, 0);
    integer_type _datatype = ::sqlite3_column_int64(_stmt, 1);
    ::sqlite3_finalize(_stmt);

    return _system.addParameterType(_id
                                    , _name
                                    , getDataType(_datatype));
}

ParameterType & HardwareStorage::getParameterType(System & _system
                                                  , integer_type _id) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT ParameterType.name, ParameterType.datatype"
                               " FROM ParameterType"
                               " WHERE ParameterType.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get ParameterType " << _id
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    std::string _name = (char const *)(::sqlite3_column_text(_stmt, 0));
    integer_type _datatype = ::sqlite3_column_int64(_stmt, 1);
    ::sqlite3_finalize(_stmt);

    return _system.addParameterType(_id
                                    , _name
                                    , getDataType(_datatype));
}

Parameter const & HardwareStorage::registerParameter(DeviceType & _devicetype
                                                     , ParameterType const & _parametertype
                                                     , integer_type _position
                                                     , unsigned char _properties)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parameter_, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parameter_, 2, _parametertype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parameter_, 3, _position)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int(insert_parameter_, 4, ((_properties & Parameter::is_property_) ? 1 : 0))) != SQLITE_OK
        || (_return = ::sqlite3_bind_int(insert_parameter_, 5, ((_properties & Parameter::is_observable_) ? 1 : 0))) != SQLITE_OK
        || (_return = ::sqlite3_bind_int(insert_parameter_, 6, ((_properties & Parameter::is_configurable_) ? 1 : 0))) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parameter_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parameter_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getParameter(_devicetype, _parametertype, _position);
            else
                {
                    std::ostringstream _errormsg;
                    _errormsg << "Could not register Parameter "
                              << _devicetype.getName() << "::"
                              << _parametertype.getName() << " "
                              << _position
                              << ": " << ::sqlite3_errmsg(db_);
                    throw std::runtime_error(_errormsg.str());
                }
        }
    ::sqlite3_reset(insert_parameter_);

    return _devicetype.addParameter(::sqlite3_last_insert_rowid(db_)
                                    , _parametertype
                                    , _position
                                    , _properties);
}

Parameter const & HardwareStorage::getParameter(DeviceType & _devicetype
                                                , ParameterType const & _parametertype
                                                , integer_type _position) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT id, property, observable, configurable"
                               " FROM Parameter"
                               " WHERE devicetype=? AND parametertype=? AND position=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 2, _parametertype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 3, _position)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get Parameter "
                      << _parametertype.getName() << " "
                      << _position
                      << " for " << _devicetype.getName() << " "
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    integer_type _id = ::sqlite3_column_int64(_stmt, 0);
    bool _property     = ::sqlite3_column_int(_stmt, 1);
    bool _observable   = ::sqlite3_column_int(_stmt, 2);
    bool _configurable = ::sqlite3_column_int(_stmt, 3);

    unsigned char _properties
        = (_property ? Parameter::is_property_ : 0x0)
        | (_observable ? Parameter::is_observable_ : 0x0)
        | (_configurable ? Parameter::is_configurable_ : 0x0);

    ::sqlite3_finalize(_stmt);

    return _devicetype.addParameter(_id
                                    , _parametertype
                                    , _position
                                    , _properties);
}

Parameter const & HardwareStorage::getParameter(DeviceType & _devicetype
                                                , integer_type _id) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT ParameterType.name"
                               ", Parameter.position"
                               ", Parameter.property, Parameter.observable, Parameter.configurable"
                               " FROM Parameter"
                               " INNER JOIN ParameterType ON ParameterType.id=Parameter.parametertype"
                               " WHERE Parameter.devicetype=? AND Parameter.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 2, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get Parameter for "
                      << _devicetype.getName()
                      << " with id " << _id
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    std::string _parametertype_name = (char const *)(::sqlite3_column_text(_stmt, 0));
    integer_type _position = ::sqlite3_column_int64(_stmt, 1);
    bool _property     = ::sqlite3_column_int(_stmt, 2);
    bool _observable   = ::sqlite3_column_int(_stmt, 3);
    bool _configurable = ::sqlite3_column_int(_stmt, 4);

    unsigned char _properties
        = (_property ? Parameter::is_property_ : 0x0)
        | (_observable ? Parameter::is_observable_ : 0x0)
        | (_configurable ? Parameter::is_configurable_ : 0x0);

    ::sqlite3_finalize(_stmt);

    return _devicetype.addParameter(_id
                                    , _devicetype.getSystem().getParameterType(_parametertype_name)
                                    , _position
                                    , _properties);
}

Service & HardwareStorage::registerService(System & _system
                                           , std::string const & _name)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_text(insert_service_, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_service_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_service_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getService(_system, _name);
            else
                throw std::runtime_error(std::string("Could not register Service ")
                                         + _name
                                         + " for System " + _system.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_service_);

    return _system.addService(::sqlite3_last_insert_rowid(db_), _name);
}

Service & HardwareStorage::getService(System & _system
                                      , std::string const & _name) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT Service.id"
                               " FROM Service"
                               " WHERE Service.name=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_text(_stmt, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            throw std::runtime_error(std::string("Could not get Service ")
                                     + _name
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    integer_type _id = ::sqlite3_column_int64(_stmt, 0);
    ::sqlite3_finalize(_stmt);

    return _system.addService(_id, _name);
}

Service & HardwareStorage::getService(System & _system
                                      , integer_type _id) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT Service.name"
                               " FROM Service"
                               " WHERE Service.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get Service " << _id
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    std::string _name = (char const *)(::sqlite3_column_text(_stmt, 0));
    ::sqlite3_finalize(_stmt);

    return _system.addService(_id, _name);
}

Connector const & HardwareStorage::registerConnector(DeviceType & _devicetype
                                                     , Service const & _service
                                                     , integer_type _position
                                                     , Direction _direction)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_connector_, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_connector_, 2, _service.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_connector_, 3, _position)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int(insert_connector_, 4, _direction)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_connector_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_connector_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getConnector(_devicetype, _service, _position, _direction);
            else
                {
                    std::ostringstream _errormsg;
                    _errormsg << "Could not register Connector "
                              << _devicetype.getName() << "::"
                              << _service.getName() << " "
                              << _position
                              << ": " << ::sqlite3_errmsg(db_);
                    throw std::runtime_error(_errormsg.str());
                }
        }
    ::sqlite3_reset(insert_connector_);

    return _devicetype.addConnector(::sqlite3_last_insert_rowid(db_)
                                    , _service
                                    , _position
                                    , _direction);
}

Connector const & HardwareStorage::getConnector(DeviceType & _devicetype
                                                , Service const & _service
                                                , integer_type _position
                                                , Direction _direction) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT id"
                               " FROM Connector"
                               " WHERE devicetype=? AND service=? AND position=? AND direction=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 2, _service.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 3, _position)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int(_stmt, 4, _direction)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get Connector "
                      << _devicetype.getName() << " "
                      << _service.getName() << " "
                      << _position
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    integer_type _id = ::sqlite3_column_int64(_stmt, 0);

    ::sqlite3_finalize(_stmt);

    return _devicetype.addConnector(_id
                                    , _service
                                    , _position
                                    , _direction);
}

Connector const & HardwareStorage::getConnector(DeviceType & _devicetype
                                                , integer_type _id) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT Service.name, Connector.position, Connector.direction"
                               " FROM Connector"
                               " INNER JOIN Service ON Service.id=Connector.service"
                               " WHERE Service.devicetype=? AND Service.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 2, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get Connector "
                      << _devicetype.getName()
                      << " with id " << _id
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    std::string _service_name = (char const *)(::sqlite3_column_text(_stmt, 0));
    integer_type _position = ::sqlite3_column_int64(_stmt, 1);
    Direction _direction = (Direction)(::sqlite3_column_int(_stmt, 2));

    ::sqlite3_finalize(_stmt);

    return _devicetype.addConnector(_id
                                    , _devicetype.getSystem().getService(_service_name)
                                    , _position
                                    , _direction);
}

Configuration HardwareStorage::registerConfiguration(System & _system, std::string const & _name)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_text(insert_configuration_, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_configuration_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_configuration_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getConfiguration(_system, _name);
            else
                throw std::runtime_error(std::string("Could not register Configuration ")
                                         + _name
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_configuration_);

    return Configuration(_system
                         , ::sqlite3_last_insert_rowid(db_)
                         , _name);
}

Configuration HardwareStorage::getConfiguration(System & _system, std::string const & _name)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT Configuration.id"
                               " FROM Configuration"
                               " WHERE Configuration.name=?"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_text(_stmt, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            throw std::runtime_error(std::string("Could not load Configuration ")
                                     + _name
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    integer_type _id = ::sqlite3_column_int64(_stmt, 0);
    ::sqlite3_finalize(_stmt);

    return Configuration(_system, _id, _name);
}

void HardwareStorage::loadConfiguration(Configuration & _configuration
                                        , rpct::tools::Time const & _time) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    _configuration.reset();
    { // DeviceTypeConfigurations
        int _return;
        if ((_return = ::sqlite3_bind_int64(load_devicetypeconfigurations_, 1, _configuration.getSystem().getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_devicetypeconfigurations_, 2, _configuration.getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_devicetypeconfigurations_, 3, _time.ustime())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_devicetypeconfigurations_, 4, _configuration.getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_devicetypeconfigurations_, 5, DataType::float_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_devicetypeconfigurations_, 6, DataType::text_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_devicetypeconfigurations_, 7, DataType::blob_id_)) != SQLITE_OK)
            {
                ::sqlite3_reset(load_devicetypeconfigurations_);
                throw std::runtime_error(std::string("Could not load Configuration ")
                                         + _configuration.getName() + " DeviceConfigurations: "
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _devicetypeconfiguration_id
            , _devicetype_id;
        std::string _devicetype_name;
        integer_type _parameter_id
            , _datatype_id
            , _integer_value;
        float_type _float_value(0.);
        text_type _text_value;
        blob_type _blob_value;

        DeviceType const * _devicetype(0);
        DeviceConfiguration * _devicetypeconfiguration(0);

        while ((_return = ::sqlite3_step(load_devicetypeconfigurations_)) == SQLITE_ROW)
            {
                _devicetypeconfiguration_id = ::sqlite3_column_int64(load_devicetypeconfigurations_, 0);
                _devicetype_id = ::sqlite3_column_int64(load_devicetypeconfigurations_, 1);
                _devicetype_name = (char const *)(::sqlite3_column_text(load_devicetypeconfigurations_, 2));

                if (!_devicetype || _devicetype->getId() != _devicetype_id)
                    _devicetype
                        = &(_configuration.getSystem().addDeviceType(_devicetype_id, _devicetype_name));
                if (!_devicetypeconfiguration || _devicetypeconfiguration->getId() != _devicetypeconfiguration_id)
                    _devicetypeconfiguration
                        = &(_configuration.addDeviceTypeConfiguration(_devicetypeconfiguration_id
                                                                      , *_devicetype));
                if (::sqlite3_column_type(load_devicetypeconfigurations_, 3) != SQLITE_NULL) {
                    _parameter_id = ::sqlite3_column_int64(load_devicetypeconfigurations_, 3);
                    Parameter const & _parameter = _devicetype->getParameter(_parameter_id);

                    _datatype_id = ::sqlite3_column_int64(load_devicetypeconfigurations_, 4);
                    _integer_value = ::sqlite3_column_int64(load_devicetypeconfigurations_, 5);

                    switch (_datatype_id)
                        {
                        case DataType::integer_id_:
                            _devicetypeconfiguration->addParameterSetting(_parameter
                                                                          , _integer_value);
                            break;
                        case DataType::float_id_:
                            _float_value = ::sqlite3_column_double(load_devicetypeconfigurations_, 6);
                            _devicetypeconfiguration->addParameterSetting(_parameter
                                                                          , _integer_value
                                                                          , _float_value);
                            break;
                        case DataType::text_id_:
                            _text_value = (char const *)(::sqlite3_column_text(load_devicetypeconfigurations_, 7));

                            _devicetypeconfiguration->addParameterSetting(_parameter
                                                                          , _integer_value
                                                                          , _text_value);
                            break;
                        case DataType::blob_id_:
                            {
                                int _size = ::sqlite3_column_bytes(load_devicetypeconfigurations_, 8);
                                unsigned char const * _data
                                    = (unsigned char const *)(::sqlite3_column_blob(load_devicetypeconfigurations_, 8));
                                _blob_value.assign(_data, _data + _size);
                            }
                            _devicetypeconfiguration->addParameterSetting(_parameter
                                                                          , _integer_value
                                                                          , _blob_value);
                            break;
                        }
                }
            }
        ::sqlite3_reset(load_devicetypeconfigurations_);
    }

    { // device to devicetypeconfiguration
        int _return;
        if ((_return = ::sqlite3_bind_int64(load_deviceconfigurations_, 1, _configuration.getSystem().getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_deviceconfigurations_, 2, _configuration.getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_deviceconfigurations_, 3, _time.ustime())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_deviceconfigurations_, 4, _configuration.getId())) != SQLITE_OK)
            {
                ::sqlite3_reset(load_deviceconfigurations_);
                throw std::runtime_error(std::string("Could not load Configuration ")
                                         + _configuration.getName() + " DeviceConfigurations: "
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _device_id;
        integer_type _devicetypeconfiguration_id;

        while ((_return = ::sqlite3_step(load_deviceconfigurations_)) == SQLITE_ROW)
            {
                _device_id = ::sqlite3_column_int64(load_deviceconfigurations_, 0);
                _devicetypeconfiguration_id = ::sqlite3_column_int64(load_deviceconfigurations_, 1);
                _configuration.addDeviceConfiguration(_device_id, _devicetypeconfiguration_id);
            }
        ::sqlite3_reset(load_deviceconfigurations_);
    }
}

DeviceConfiguration & HardwareStorage::registerDeviceTypeConfiguration(Configuration & _configuration
                                                                       , DeviceType const & _devicetype)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_devicetypeconfiguration_, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_devicetypeconfiguration_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_devicetypeconfiguration_);
            throw std::runtime_error(std::string("Could not register DeviceTypeConfiguration ")
                                     + "for " + _devicetype.getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_devicetypeconfiguration_);

    return _configuration.addDeviceTypeConfiguration(::sqlite3_last_insert_rowid(db_), _devicetype);
}

DeviceConfiguration & HardwareStorage::getDeviceTypeConfiguration(Configuration & _configuration
                                                                  , integer_type _id) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceType.name"
                               " FROM DeviceTypeConfiguration"
                               " INNER JOIN DeviceType ON (DeviceTypeConfiguration.devicetype=DeviceType.id)"
                               " WHERE DeviceTypeConfiguration.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not load DeviceTypeConfiguration "
                      << _id
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    std::string _devicetype_name = (char const *)(::sqlite3_column_text(_stmt, 0));
    ::sqlite3_finalize(_stmt);

    DeviceConfiguration & _deviceconfiguration
        = _configuration.addDeviceTypeConfiguration(_id
                                                    , _configuration.getSystem().getDeviceType(_devicetype_name));
    loadDeviceTypeConfiguration(_deviceconfiguration);
    return _deviceconfiguration;
}

StandaloneDeviceConfiguration HardwareStorage::registerDeviceTypeConfiguration(System & _system
                                                                               , DeviceType const & _devicetype)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_devicetypeconfiguration_, 1, _devicetype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_devicetypeconfiguration_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_devicetypeconfiguration_);
            throw std::runtime_error(std::string("Could not register DeviceTypeConfiguration ")
                                     + "for " + _devicetype.getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_devicetypeconfiguration_);

    return StandaloneDeviceConfiguration(_system
                                         , ::sqlite3_last_insert_rowid(db_)
                                         , _devicetype);
}

StandaloneDeviceConfiguration HardwareStorage::getDeviceTypeConfiguration(System & _system
                                                                          , integer_type _id)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT DeviceType.name"
                               " FROM DeviceTypeConfiguration"
                               " INNER JOIN DeviceType ON (DeviceTypeConfiguration.devicetype=DeviceType.id)"
                               " WHERE DeviceTypeConfiguration.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not load DeviceTypeConfiguration "
                      << _id
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    std::string _devicetype_name = (char const *)(::sqlite3_column_text(_stmt, 0));
    ::sqlite3_finalize(_stmt);

    StandaloneDeviceConfiguration _deviceconfiguration(_system, _id, _system.getDeviceType(_devicetype_name));
    loadDeviceTypeConfiguration(_deviceconfiguration);
    return _deviceconfiguration;
}

void HardwareStorage::loadDeviceTypeConfiguration(DeviceConfiguration & _devicetypeconfiguration) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    { // DeviceTypeConfigurations
        ::sqlite3_stmt * _stmt;
        int _return
            = ::sqlite3_prepare_v2(db_
                                   , "SELECT Parameter.id"
                                   ", ParameterSetting.setting"
                                   ", ParameterType.datatype"
                                   ", ParameterSettingFloat.value, ParameterSettingText.value, ParameterSettingBlob.value"
                                   " FROM ParameterSetting"
                                   " INNER JOIN Parameter ON (ParameterSetting .parameter=Parameter.id)"
                                   " INNER JOIN ParameterType ON (Parameter.parametertype=ParameterType.id)"
                                   " LEFT OUTER JOIN ParameterSettingFloat"
                                   "   ON (ParameterSetting.setting=ParameterSettingFloat.id AND ParameterType.datatype=?)"
                                   " LEFT OUTER JOIN ParameterSettingText"
                                   "   ON (ParameterSetting.setting=ParameterSettingText.id AND ParameterType.datatype=?)"
                                   " LEFT OUTER JOIN ParameterSettingBlob"
                                   "   ON (ParameterSetting.setting=ParameterSettingBlob.id AND ParameterType.datatype=?)"
                                   " WHERE (ParameterSetting.devicetypeconfiguration=?)"
                                   , -1, &_stmt, 0);
        if (_return != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 1, DataType::float_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 2, DataType::text_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 3, DataType::blob_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(_stmt, 4, _devicetypeconfiguration.getId())) != SQLITE_OK)
            {
                ::sqlite3_finalize(_stmt);
                std::ostringstream _errormsg;
                _errormsg << "Could not load DeviceTypeConfiguration " << _devicetypeconfiguration.getId()
                          << ": " << ::sqlite3_errmsg(db_);
                throw std::runtime_error(_errormsg.str());
            }

        integer_type _parameter_id, _datatype_id;
        integer_type _integer_value;
        float_type _float_value(0.);
        text_type _text_value;
        blob_type _blob_value;

        DeviceType const & _devicetype = _devicetypeconfiguration.getDeviceType();
        while ((_return = ::sqlite3_step(_stmt)) == SQLITE_ROW)
            {
                _parameter_id = ::sqlite3_column_int64(_stmt, 0);
                _integer_value = ::sqlite3_column_int64(_stmt, 1);
                _datatype_id = ::sqlite3_column_int64(_stmt, 2);
                Parameter const & _parameter = _devicetype.getParameter(_parameter_id);
                switch (_datatype_id)
                    {
                    case DataType::integer_id_:
                        _devicetypeconfiguration.addParameterSetting(_parameter, _integer_value);
                        break;
                    case DataType::float_id_:
                        _float_value = ::sqlite3_column_double(_stmt, 3);
                        _devicetypeconfiguration.addParameterSetting(_parameter
                                                                     , _integer_value
                                                                     , _float_value);
                        break;
                    case DataType::text_id_:
                        _text_value = (char const *)(::sqlite3_column_text(_stmt, 4));
                        _devicetypeconfiguration.addParameterSetting(_parameter
                                                                     , _integer_value
                                                                     , _text_value);
                        break;
                    case DataType::blob_id_:
                        int _size = ::sqlite3_column_bytes(_stmt, 5);
                        unsigned char const * _data
                            = (unsigned char const *)(::sqlite3_column_blob(_stmt, 5));
                        _blob_value.assign(_data, _data + _size);
                        _devicetypeconfiguration.addParameterSetting(_parameter
                                                                     , _integer_value
                                                                     , _blob_value);
                        break;
                    }
            }
        ::sqlite3_finalize(_stmt);
    }
}

void HardwareStorage::registerDeviceConfiguration(Configuration & _configuration
                                                  , Device const & _device
                                                  , DeviceConfiguration const & _devicetypeconfiguration)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    integer_type _time = rpct::tools::Time().ustime();

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 1, _configuration.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 2, _device.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 3, _devicetypeconfiguration.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 4, _time)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_deviceconfiguration_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_deviceconfiguration_);
            std::ostringstream _errormsg;
            _errormsg << "Could not register DeviceConfiguration " << _devicetypeconfiguration.getId()
                      << " in configuration " << _configuration.getName()
                      << " for " << _device.getId()
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    ::sqlite3_reset(insert_deviceconfiguration_);

    if (_devicetypeconfiguration.getConfiguration().getId() != _configuration.getId())
        getDeviceTypeConfiguration(_configuration, _devicetypeconfiguration.getId());

    return _configuration.addDeviceConfiguration(_device, _devicetypeconfiguration);
}

integer_type HardwareStorage::registerParameterSetting(integer_type _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    return _value;
}

integer_type HardwareStorage::registerParameterSetting(float_type _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_double(insert_parametersettingfloat_, 1, _value)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametersettingfloat_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametersettingfloat_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getParameterSetting(_value);
            else
                {
                    std::ostringstream _errormsg;
                    _errormsg << "Could not register ParameterSetting "
                              << _value
                              << ": " << ::sqlite3_errmsg(db_);
                    throw std::runtime_error(_errormsg.str());
                }
        }
    ::sqlite3_reset(insert_parametersettingfloat_);

    return ::sqlite3_last_insert_rowid(db_);
}

integer_type HardwareStorage::getParameterSetting(float_type _value) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_double(select_parametersettingfloat_, 1, _value)) != SQLITE_OK
        || (_return = ::sqlite3_step(select_parametersettingfloat_)) != SQLITE_ROW)
        {
            ::sqlite3_reset(select_parametersettingfloat_);
            std::ostringstream _errormsg;
            _errormsg << "Could not get ParameterSetting "
                      << _value
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    integer_type _id = ::sqlite3_column_int64(select_parametersettingfloat_, 0);
    ::sqlite3_reset(select_parametersettingfloat_);
    return _id;
}

integer_type HardwareStorage::registerParameterSetting(text_type const & _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    integer_type _hash = rpct::tools::Murmur3a::hash(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(select_parametersettingtext_, 1, _hash)) != SQLITE_OK)
        {
            ::sqlite3_reset(select_parametersettingtext_);
            throw std::runtime_error(std::string("Could not register ParameterSetting ")
                                     + _value
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    while (::sqlite3_step(select_parametersettingtext_) == SQLITE_ROW)
        {
            size_t _size = sqlite3_column_bytes(select_parametersettingtext_, 1);
            if (_size == _value.size()
                && !(std::memcmp((void const *)(sqlite3_column_text(select_parametersettingtext_, 1))
                                 , (void const *)(&(_value[0]))
                                 , _size)
                     )
                )
                {
                    integer_type _id = ::sqlite3_column_int64(select_parametersettingtext_, 0);
                    ::sqlite3_reset(select_parametersettingtext_);
                    return _id;
                }
        }
    ::sqlite3_reset(select_parametersettingtext_);

    // it doesn't exist yet - insert
    if ((_return = ::sqlite3_bind_text(insert_parametersettingtext_, 1, _value.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersettingtext_, 2, _hash)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametersettingtext_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametersettingtext_);
            throw std::runtime_error(std::string("Could not register ParameterSetting ")
                                     + _value
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametersettingtext_);

    return ::sqlite3_last_insert_rowid(db_);
}

integer_type HardwareStorage::registerParameterSetting(blob_type const & _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    integer_type _hash = rpct::tools::Murmur3a::hash(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(select_parametersettingblob_, 1, _hash)) != SQLITE_OK)
        {
            ::sqlite3_reset(select_parametersettingblob_);
            throw std::runtime_error(std::string("Could not register ParameterSetting ")
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    while (::sqlite3_step(select_parametersettingblob_) == SQLITE_ROW)
        {
            size_t _size = sqlite3_column_bytes(select_parametersettingblob_, 1);
            if (_size == _value.size()
                && !(std::memcmp(sqlite3_column_blob(select_parametersettingblob_, 1)
                                 , (void const *)(&(_value[0]))
                                 , _size)
                     )
                )
                {
                    integer_type _id = ::sqlite3_column_int64(select_parametersettingblob_, 0);
                    ::sqlite3_reset(select_parametersettingblob_);
                    return _id;
                }
        }
    ::sqlite3_reset(select_parametersettingblob_);

    // it doesn't exist yet - insert
    void const * _address = (void const *)(&(_value[0]));
    if (_value.size() == 0)
        _address = (void const *)(&_value); // put any value here, just not null
    if ((_return = ::sqlite3_bind_blob(insert_parametersettingblob_, 1
                                       , _address
                                       , _value.size()
                                       , SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersettingblob_, 2, _hash)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametersettingblob_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametersettingblob_);
            throw std::runtime_error(std::string("Could not register ParameterSetting ")
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametersettingblob_);

    return ::sqlite3_last_insert_rowid(db_);
}

void HardwareStorage::registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                               , Parameter const & _parameter
                                               , integer_type _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERSETTING", 0, 0, 0);

    integer_type _parametersetting = registerParameterSetting(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametersetting_, 1, _devicetypeconfiguration.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 3, _parametersetting)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametersetting_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametersetting_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERSETTING", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterSetting")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametersetting_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERSETTING", 0, 0, 0);

    _devicetypeconfiguration.addParameterSetting(_parameter, _value);
}

void HardwareStorage::registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                               , Parameter const & _parameter
                                               , float_type _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERSETTING", 0, 0, 0);

    integer_type _parametersetting = registerParameterSetting(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametersetting_, 1, _devicetypeconfiguration.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 3, _parametersetting)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametersetting_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametersetting_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERSETTING", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterSetting")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametersetting_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERSETTING", 0, 0, 0);

    _devicetypeconfiguration.addParameterSetting(_parameter
                                                 , _parametersetting
                                                 , _value);
}


void HardwareStorage::registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                               , Parameter const & _parameter
                                               , text_type const & _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERSETTING", 0, 0, 0);

    integer_type _parametersetting = registerParameterSetting(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametersetting_, 1, _devicetypeconfiguration.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 3, _parametersetting)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametersetting_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametersetting_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERSETTING", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterSetting")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametersetting_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERSETTING", 0, 0, 0);

    _devicetypeconfiguration.addParameterSetting(_parameter
                                                 , _parametersetting
                                                 , _value);
}

void HardwareStorage::registerParameterSetting(DeviceConfiguration & _devicetypeconfiguration
                                               , Parameter const & _parameter
                                               , blob_type const & _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERSETTING", 0, 0, 0);

    integer_type _parametersetting = registerParameterSetting(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametersetting_, 1, _devicetypeconfiguration.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametersetting_, 3, _parametersetting)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametersetting_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametersetting_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERSETTING", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterSetting")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametersetting_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERSETTING", 0, 0, 0);

    _devicetypeconfiguration.addParameterSetting(_parameter
                                                 , _parametersetting
                                                 , _value);
}

Device & HardwareStorage::registerDevice(System & _system
                                         , DeviceType const & _devicetype
                                         , DeviceConfiguration const & _properties)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTDEVICE", 0, 0, 0);

    int _return;
    { // The Device
        if ((_return = ::sqlite3_bind_int64(insert_device_, 1, _devicetype.getId())) != SQLITE_OK
            || (_return = ::sqlite3_step(insert_device_)) != SQLITE_DONE)
            {
                ::sqlite3_reset(insert_device_);
                ::sqlite3_exec(db_, "ROLLBACK TO INSERTDEVICE", 0, 0, 0);
                throw std::runtime_error(std::string("Could not register Device with type ")
                                         + _devicetype.getName()
                                         + " for System " + _system.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_reset(insert_device_);
    }

    integer_type _id = ::sqlite3_last_insert_rowid(db_);

    { // The System link
        if ((_return = ::sqlite3_bind_int64(insert_systemdevice_, 1, _system.getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(insert_systemdevice_, 2, _id)) != SQLITE_OK
            || (_return = ::sqlite3_step(insert_systemdevice_)) != SQLITE_DONE)
            {
                ::sqlite3_reset(insert_systemdevice_);
                ::sqlite3_exec(db_, "ROLLBACK TO INSERTDEVICE", 0, 0, 0);
                throw std::runtime_error(std::string("Could not register Device with type ")
                                         + _devicetype.getName()
                                         + " for System " + _system.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_reset(insert_systemdevice_);
    }

    { // The Properties link
        rpct::tools::Time _time;
        if ((_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 1, 0)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 2, _id)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 3, _properties.getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(insert_deviceconfiguration_, 4, _time.ustime())) != SQLITE_OK
            || (_return = ::sqlite3_step(insert_deviceconfiguration_)) != SQLITE_DONE)
            {
                ::sqlite3_reset(insert_deviceconfiguration_);
                ::sqlite3_exec(db_, "ROLLBACK TO INSERTDEVICE", 0, 0, 0);
                throw std::runtime_error(std::string("Could not register Device with type ")
                                         + _devicetype.getName()
                                         + " for System " + _system.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        ::sqlite3_reset(insert_deviceconfiguration_);
    }

    ::sqlite3_exec(db_, "RELEASE INSERTDEVICE", 0, 0, 0);

    return _system.addDevice(_id, _devicetype, _properties);
}

Connection HardwareStorage::registerConnection(Device & _src
                                               , Connector const & _srcconnector
                                               , Device & _dest
                                               , Connector const & _destconnector)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_connection_, 1, _src.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_connection_, 2, _srcconnector.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_connection_, 3, _dest.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_connection_, 4, _destconnector.getId())) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_connection_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_connection_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getConnection(_src, _srcconnector, _dest, _destconnector);
            else
                throw std::runtime_error(std::string("Could not register Connection ")
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_connection_);

    Connection _connection(::sqlite3_last_insert_rowid(db_)
                           , _src, _srcconnector, _dest, _destconnector);
    _src.addConnection(_connection);
    _dest.addConnection(_connection);

    return _connection;
}

Connection HardwareStorage::getConnection(Device & _src
                                          , Connector const & _srcconnector
                                          , Device & _dest
                                          , Connector const & _destconnector) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT Connection.id"
                               " FROM Connection"
                               " WHERE src=? AND srcconnector=? AND dest=? AND destconnector=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _src.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 2, _srcconnector.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 3, _dest.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 4, _destconnector.getId())) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            throw std::runtime_error(std::string("Could not get Connection ")
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    integer_type _id = ::sqlite3_column_int64(_stmt, 0);
    ::sqlite3_finalize(_stmt);

    Connection _connection(_id
                           , _src, _srcconnector, _dest, _destconnector);
    _src.addConnection(_connection);
    _dest.addConnection(_connection);

    return _connection;
}

SnapshotType & HardwareStorage::registerSnapshotType(System & _system
                                                     , std::string const & _name)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_text(insert_snapshottype_, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_snapshottype_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_snapshottype_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getSnapshotType(_system, _name);
            else
                throw std::runtime_error(std::string("Could not register SnapshotType ")
                                         + _name
                                         + " for System " + _system.getName()
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_snapshottype_);

    return _system.addSnapshotType(::sqlite3_last_insert_rowid(db_), _name);
}

SnapshotType & HardwareStorage::getSnapshotType(System & _system
                                                , std::string const & _name) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_text(select_snapshottype_, 1, _name.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_step(select_snapshottype_)) != SQLITE_ROW)
        {
            ::sqlite3_reset(select_snapshottype_);
            throw std::runtime_error(std::string("Could not get SnapshotType ")
                                     + _name
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    integer_type _id = ::sqlite3_column_int64(select_snapshottype_, 0);
    ::sqlite3_reset(select_snapshottype_);

    return _system.addSnapshotType(_id, _name);
}

SnapshotType & HardwareStorage::getSnapshotType(System & _system
                                                , integer_type _id) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_stmt * _stmt;
    int _return
        = ::sqlite3_prepare_v2(db_
                               , "SELECT SnapshotType.name"
                               " FROM SnapshotType"
                               " WHERE SnapshotType.id=?"
                               " LIMIT 1"
                               , -1, &_stmt, 0);
    if (_return != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(_stmt, 1, _id)) != SQLITE_OK
        || (_return = ::sqlite3_step(_stmt)) != SQLITE_ROW)
        {
            ::sqlite3_finalize(_stmt);
            std::ostringstream _errormsg;
            _errormsg << "Could not get SnapshotType " << _id
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    std::string _name = (char const *)(::sqlite3_column_text(_stmt, 0));
    ::sqlite3_finalize(_stmt);

    return _system.addSnapshotType(_id, _name);
}

Snapshot HardwareStorage::takeSnapshot(System const & _system
                                       , SnapshotType const & _snapshottype
                                       , rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_snapshot_, 1, _snapshottype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_snapshot_, 2, _system.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_snapshot_, 3, _time.ustime())) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_snapshot_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_snapshot_);
            std::ostringstream _errormsg;
            _errormsg << "Could not register Snapshot "
                      << _snapshottype.getName()
                      << " for " << _system.getName()
                      << " at " << rpct::tools::Date(_time)
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    ::sqlite3_reset(insert_snapshot_);

    return Snapshot(_system
                    , ::sqlite3_last_insert_rowid(db_)
                    , _snapshottype
                    , _time);
}

std::vector<Snapshot> HardwareStorage::getSnapshots(System const & _system
                                                    , SnapshotType const & _snapshottype
                                                    , rpct::tools::Time const & _start
                                                    , rpct::tools::Time const & _stop) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(select_snapshots_, 1, _snapshottype.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(select_snapshots_, 2, _system.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(select_snapshots_, 3, _start.ustime() + 1)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(select_snapshots_, 4, _stop.ustime())) != SQLITE_OK)
        {
            ::sqlite3_reset(select_snapshots_);
            std::ostringstream _errormsg;
            _errormsg << "Could not get Snapshots of type "
                      << _snapshottype.getName()
                      << " for " << _system.getName()
                      << " between " << rpct::tools::Date(_start)
                      << " and " << rpct::tools::Date(_stop)
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }

    std::vector<Snapshot> _snapshots;
    integer_type _id, _time;

    while ((_return = ::sqlite3_step(select_snapshots_)) == SQLITE_ROW)
        {
            _id   = ::sqlite3_column_int64(select_snapshots_, 0);
            _time = ::sqlite3_column_int64(select_snapshots_, 1);
            _snapshots.push_back(Snapshot(_system
                                          , _id
                                          , _snapshottype
                                          , rpct::tools::Time::ustime(_time)));
        }

    ::sqlite3_reset(select_snapshots_);

    return _snapshots;
}

void HardwareStorage::loadSnapshotConfiguration(Configuration & _configuration
                                                , rpct::tools::Time const & _snapshot
                                                , rpct::tools::Time const & _previous) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    _configuration.reset();

    { // DeviceTypeConfigurations
        int _return;
        if ((_return = ::sqlite3_bind_int64(load_snapshot_devicetypeconfigurations_, 1, _configuration.getSystem().getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_devicetypeconfigurations_, 2, _previous.ustime() + 1)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_devicetypeconfigurations_, 3, _snapshot.ustime())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_devicetypeconfigurations_, 4, DataType::float_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_devicetypeconfigurations_, 5, DataType::text_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_devicetypeconfigurations_, 6, DataType::blob_id_)) != SQLITE_OK)
            {
                ::sqlite3_reset(load_snapshot_devicetypeconfigurations_);
                throw std::runtime_error(std::string("Could not load SnapshotConfiguration ")
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _devicetypeconfiguration_id, _devicetype_id;
        std::string _devicetype_name;
        integer_type _parameter_id, _datatype_id
            , _integer_value;
        float_type _float_value(0.);
        text_type _text_value;
        blob_type _blob_value;

        DeviceType const * _devicetype(0);
        DeviceConfiguration * _devicetypeconfiguration(0);

        while ((_return = ::sqlite3_step(load_snapshot_devicetypeconfigurations_)) == SQLITE_ROW)
            {
                _devicetypeconfiguration_id = ::sqlite3_column_int64(load_snapshot_devicetypeconfigurations_, 0);
                _devicetype_id = ::sqlite3_column_int64(load_snapshot_devicetypeconfigurations_, 1);
                if (!_devicetype || _devicetype->getId() != _devicetype_id)
                    {
                        _devicetype_name = (char const *)(::sqlite3_column_text(load_snapshot_devicetypeconfigurations_, 2));
                        _devicetype
                            = &(_configuration.getSystem().getDeviceType(_devicetype_name));
                    }
                if (!_devicetypeconfiguration || _devicetypeconfiguration->getId() != _devicetypeconfiguration_id)
                    {
                        _devicetypeconfiguration
                            = &(_configuration.addDeviceTypeConfiguration(_devicetypeconfiguration_id
                                                                          , *_devicetype));
                    }
                _parameter_id = ::sqlite3_column_int64(load_snapshot_devicetypeconfigurations_, 3);
                Parameter const & _parameter = _devicetype->getParameter(_parameter_id);

                _datatype_id = ::sqlite3_column_int64(load_snapshot_devicetypeconfigurations_, 4);
                _integer_value = ::sqlite3_column_int64(load_snapshot_devicetypeconfigurations_, 5);
                switch (_datatype_id)
                    {
                    case DataType::integer_id_:
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value);
                        break;
                    case DataType::float_id_:
                        _float_value = ::sqlite3_column_double(load_snapshot_devicetypeconfigurations_, 6);
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value
                                                                      , _float_value);
                        break;
                    case DataType::text_id_:
                        _text_value = (char const *)(::sqlite3_column_text(load_snapshot_devicetypeconfigurations_, 7));
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value
                                                                      , _text_value);
                        break;
                    case DataType::blob_id_:
                        int _size = ::sqlite3_column_bytes(load_snapshot_devicetypeconfigurations_, 8);
                        unsigned char const * _data
                            = (unsigned char const *)(::sqlite3_column_blob(load_snapshot_devicetypeconfigurations_, 8));
                        _blob_value.assign(_data, _data + _size);
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value
                                                                      , _blob_value);
                        break;
                    }
            }
        ::sqlite3_reset(load_snapshot_devicetypeconfigurations_);
    }

    { // device to devicetypeconfiguration
        int _return;
        if ((_return = ::sqlite3_bind_int64(load_snapshot_deviceconfigurations_, 1, _configuration.getSystem().getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_deviceconfigurations_, 2, _previous.ustime() + 1)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_deviceconfigurations_, 3, _snapshot.ustime())) != SQLITE_OK)
            {
                ::sqlite3_reset(load_snapshot_deviceconfigurations_);
                throw std::runtime_error(std::string("Could not load Configuration ")
                                         + _configuration.getName() + " DeviceConfigurations: "
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _device_id;
        integer_type _devicetypeconfiguration_id;

        while ((_return = ::sqlite3_step(load_snapshot_deviceconfigurations_)) == SQLITE_ROW)
            {
                _device_id = ::sqlite3_column_int64(load_snapshot_deviceconfigurations_, 0);
                _devicetypeconfiguration_id = ::sqlite3_column_int64(load_snapshot_deviceconfigurations_, 1);
                _configuration.addDeviceConfiguration(_device_id, _devicetypeconfiguration_id);
            }
        ::sqlite3_reset(load_snapshot_deviceconfigurations_);
    }

}

void HardwareStorage::loadSnapshotObservables(Configuration & _configuration
                                              , rpct::tools::Time const & _snapshot
                                              , rpct::tools::Time const & _previous) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    _configuration.reset();

    { // Fake DeviceTypeConfigurations with id of device
        int _return;
        if ((_return = ::sqlite3_bind_int64(load_snapshot_deviceobservables_, 1, _configuration.getSystem().getId())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_deviceobservables_, 2, _previous.ustime() + 1)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_deviceobservables_, 3, _snapshot.ustime())) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_deviceobservables_, 4, DataType::float_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_deviceobservables_, 5, DataType::text_id_)) != SQLITE_OK
            || (_return = ::sqlite3_bind_int64(load_snapshot_deviceobservables_, 6, DataType::blob_id_)) != SQLITE_OK)
            {
                ::sqlite3_reset(load_snapshot_deviceobservables_);
                throw std::runtime_error(std::string("Could not load SnapshotObservables ")
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }

        integer_type _device_id, _devicetype_id;
        std::string _devicetype_name;
        integer_type _parameter_id, _datatype_id
            , _integer_value;
        float_type _float_value(0.);
        text_type _text_value;
        blob_type _blob_value;

        DeviceType const * _devicetype(0);
        DeviceConfiguration * _devicetypeconfiguration(0);

        while ((_return = ::sqlite3_step(load_snapshot_deviceobservables_)) == SQLITE_ROW)
            {
                // DeviceType *, _devicetype_id
                _device_id = ::sqlite3_column_int64(load_snapshot_deviceobservables_, 0);
                _devicetype_id = ::sqlite3_column_int64(load_snapshot_deviceobservables_, 1);
                if (!_devicetypeconfiguration || _devicetypeconfiguration->getId() != _device_id)
                    {
                        if (!_devicetype || _devicetype->getId() != _devicetype_id)
                            {
                                _devicetype_name = (char const *)(::sqlite3_column_text(load_snapshot_deviceobservables_, 2));
                                _devicetype = &(_configuration.getSystem().getDeviceType(_devicetype_name));
                            }
                        _devicetypeconfiguration = &(_configuration.addDeviceTypeConfiguration(_device_id
                                                                                               , *_devicetype));
                        _configuration.addDeviceConfiguration(_device_id, _device_id);
                    }

                _parameter_id = ::sqlite3_column_int64(load_snapshot_deviceobservables_, 3);
                Parameter const & _parameter = _devicetype->getParameter(_parameter_id);

                _datatype_id = ::sqlite3_column_int64(load_snapshot_deviceobservables_, 4);
                _integer_value = ::sqlite3_column_int64(load_snapshot_deviceobservables_, 5);
                switch (_datatype_id)
                    {
                    case DataType::integer_id_:
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value);
                        break;
                    case DataType::float_id_:
                        _float_value = ::sqlite3_column_double(load_snapshot_deviceobservables_, 6);
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value
                                                                      , _float_value);
                        break;
                    case DataType::text_id_:
                        _text_value = (char const *)(::sqlite3_column_text(load_snapshot_deviceobservables_, 7));
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value
                                                                      , _text_value);
                        break;
                    case DataType::blob_id_:
                        int _size = ::sqlite3_column_bytes(load_snapshot_deviceobservables_, 8);
                        unsigned char const * _data
                            = (unsigned char const *)(::sqlite3_column_blob(load_snapshot_deviceobservables_, 8));
                        _blob_value.assign(_data, _data + _size);
                        _devicetypeconfiguration->addParameterSetting(_parameter
                                                                      , _integer_value
                                                                      , _blob_value);
                        break;
                    }
            }
        ::sqlite3_reset(load_snapshot_deviceobservables_);
    }
}

void HardwareStorage::publish(Device const & _device, DeviceConfiguration const & _deviceconfiguration
                              , rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_deviceconfigurationvalue_, 1, _device.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_deviceconfigurationvalue_, 2, _deviceconfiguration.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_deviceconfigurationvalue_, 3, _time.ustime())) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_deviceconfigurationvalue_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_deviceconfigurationvalue_);
            std::ostringstream _errormsg;
            _errormsg << "Could not publish DeviceConfigurationValue "
                      << _deviceconfiguration.getId()
                      << " for Device " << _device.getId()
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    ::sqlite3_reset(insert_deviceconfigurationvalue_);
}

integer_type HardwareStorage::registerParameterValue(integer_type _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    return _value;
}

integer_type HardwareStorage::registerParameterValue(float_type _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_double(insert_parametervaluefloat_, 1, _value)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametervaluefloat_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametervaluefloat_);
            if ((_return & 0xff) == SQLITE_CONSTRAINT || (_return & 0xff) == SQLITE_READONLY)
                return getParameterValue(_value);
            else
                {
                    std::ostringstream _errormsg;
                    _errormsg << "Could not register ParameterValue "
                              << _value
                              << ": " << ::sqlite3_errmsg(db_);
                    throw std::runtime_error(_errormsg.str());
                }
        }
    ::sqlite3_reset(insert_parametervaluefloat_);

    return ::sqlite3_last_insert_rowid(db_);
}

integer_type HardwareStorage::getParameterValue(float_type _value) const
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    int _return;
    if ((_return = ::sqlite3_bind_double(select_parametervaluefloat_, 1, _value)) != SQLITE_OK
        || (_return = ::sqlite3_step(select_parametervaluefloat_)) != SQLITE_ROW)
        {
            ::sqlite3_reset(select_parametervaluefloat_);
            std::ostringstream _errormsg;
            _errormsg << "Could not get ParameterValue "
                      << _value
                      << ": " << ::sqlite3_errmsg(db_);
            throw std::runtime_error(_errormsg.str());
        }
    integer_type _id = ::sqlite3_column_int64(select_parametervaluefloat_, 0);
    ::sqlite3_reset(select_parametervaluefloat_);
    return _id;
}

integer_type HardwareStorage::registerParameterValue(text_type const & _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    integer_type _hash = rpct::tools::Murmur3a::hash(_value);

    { // check if exists
        int _return;
        if ((_return = ::sqlite3_bind_int64(select_parametervaluetext_, 1, _hash)) != SQLITE_OK)
            {
                ::sqlite3_reset(select_parametervaluetext_);
                throw std::runtime_error(std::string("Could not register ParameterValue ")
                                         + _value
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        while (::sqlite3_step(select_parametervaluetext_) == SQLITE_ROW)
            {
                size_t _size = sqlite3_column_bytes(select_parametervaluetext_, 1);
                if (_size == _value.size()
                    && !(std::memcmp((void const *)(sqlite3_column_text(select_parametervaluetext_, 1))
                                     , (void const *)(&(_value[0]))
                                     , _size)
                         )
                    )
                    {
                        integer_type _id = ::sqlite3_column_int64(select_parametervaluetext_, 0);
                        ::sqlite3_reset(select_parametervaluetext_);
                        return _id;
                    }
            }
        ::sqlite3_reset(select_parametervaluetext_);
    }
    // it doesn't exist yet - insert
    int _return;
    if ((_return = ::sqlite3_bind_text(insert_parametervaluetext_, 1, _value.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervaluetext_, 2, _hash)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametervaluetext_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametervaluetext_);
            throw std::runtime_error(std::string("Could not register ParameterValue ")
                                     + _value
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametervaluetext_);

    return ::sqlite3_last_insert_rowid(db_);
}

integer_type HardwareStorage::registerParameterValue(blob_type const & _value)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    integer_type _hash = rpct::tools::Murmur3a::hash(_value);

    { // check if exists
        int _return;
        if ((_return = ::sqlite3_bind_int64(select_parametervalueblob_, 1, _hash)) != SQLITE_OK)
            {
                ::sqlite3_reset(select_parametervalueblob_);
                throw std::runtime_error(std::string("Could not register ParameterValue ")
                                         + ": " + std::string(::sqlite3_errmsg(db_)));
            }
        while (::sqlite3_step(select_parametervalueblob_) == SQLITE_ROW)
            {
                size_t _size = sqlite3_column_bytes(select_parametervalueblob_, 1);
                if (_size == _value.size()
                    && !(std::memcmp(sqlite3_column_blob(select_parametervalueblob_, 1)
                                     , (void const *)(&(_value[0]))
                                     , _size)
                         )
                    )
                    {
                        integer_type _id = ::sqlite3_column_int64(select_parametervalueblob_, 0);
                        ::sqlite3_reset(select_parametervalueblob_);
                        return _id;
                    }
            }
        ::sqlite3_reset(select_parametervalueblob_);
    }
    // it doesn't exist yet - insert
    int _return;
    void const * _address = (void const *)(&(_value[0]));
    if (_value.size() == 0)
        _address = (void const *)(&_value); // put any value here, just not null
    if ((_return = ::sqlite3_bind_blob(insert_parametervalueblob_, 1
                                       , _address
                                       , _value.size()
                                       , SQLITE_STATIC)) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalueblob_, 2, _hash)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametervalueblob_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametervalueblob_);
            throw std::runtime_error(std::string("Could not register ParameterValue ")
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametervalueblob_);

    return ::sqlite3_last_insert_rowid(db_);
}

void HardwareStorage::registerParameterValue(Device const & _device
                                             , Parameter const & _parameter
                                             , integer_type _value
                                             , rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERVALUE", 0, 0, 0);

    integer_type _parametervalue = registerParameterValue(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametervalue_, 1, _device.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 3, _time.ustime())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 4, _parametervalue)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametervalue_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametervalue_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERVALUE", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterValue")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametervalue_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERVALUE", 0, 0, 0);
}

void HardwareStorage::registerParameterValue(Device const & _device
                                             , Parameter const & _parameter
                                             , float_type _value
                                             , rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERVALUE", 0, 0, 0);

    integer_type _parametervalue = registerParameterValue(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametervalue_, 1, _device.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 3, _time.ustime())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 4, _parametervalue)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametervalue_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametervalue_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERVALUE", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterValue")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametervalue_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERVALUE", 0, 0, 0);
}

void HardwareStorage::registerParameterValue(Device const & _device
                                             , Parameter const & _parameter
                                             , text_type const & _value
                                             , rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERVALUE", 0, 0, 0);

    integer_type _parametervalue = registerParameterValue(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametervalue_, 1, _device.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 3, _time.ustime())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 4, _parametervalue)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametervalue_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametervalue_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERVALUE", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterValue")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametervalue_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERVALUE", 0, 0, 0);
}

void HardwareStorage::registerParameterValue(Device const & _device
                                             , Parameter const & _parameter
                                             , blob_type const & _value
                                             , rpct::tools::Time const & _time)
{
    LOG4CPLUS_TRACE(logger_, "HardwareStorage::" << __FUNCTION__ << " (" << __LINE__ << ")");
    ::sqlite3_exec(db_, "SAVEPOINT INSERTPARAMETERVALUE", 0, 0, 0);

    integer_type _parametervalue = registerParameterValue(_value);

    int _return;
    if ((_return = ::sqlite3_bind_int64(insert_parametervalue_, 1, _device.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 2, _parameter.getId())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 3, _time.ustime())) != SQLITE_OK
        || (_return = ::sqlite3_bind_int64(insert_parametervalue_, 4, _parametervalue)) != SQLITE_OK
        || (_return = ::sqlite3_step(insert_parametervalue_)) != SQLITE_DONE)
        {
            ::sqlite3_reset(insert_parametervalue_);
            ::sqlite3_exec(db_, "ROLLBACK TO INSERTPARAMETERVALUE", 0, 0, 0);
            throw std::runtime_error(std::string("Could not register ParameterValue")
                                     + " for " + _parameter.getParameterType().getName()
                                     + " for System " + _parameter.getDeviceType().getSystem().getName()
                                     + ": " + std::string(::sqlite3_errmsg(db_)));
        }
    ::sqlite3_reset(insert_parametervalue_);

    ::sqlite3_exec(db_, "RELEASE INSERTPARAMETERVALUE", 0, 0, 0);
}

Transaction::Transaction(HardwareStorage & _hwstorage, bool _begin)
    : hwstorage_(_hwstorage)
    , open_(_begin)
{
    if (_begin)
        hwstorage_.beginTransaction();
}

Transaction::~Transaction()
{
    if (open_)
        hwstorage_.rollbackTransaction();
}

void Transaction::begin()
{
    if (!open_)
        {
            hwstorage_.beginTransaction();
            open_ = true;
        }
}

void Transaction::commit()
{
    if (open_)
        {
            hwstorage_.commitTransaction();
            open_ = false;
        }
}

void Transaction::rollback()
{
    if (open_)
        {
            hwstorage_.rollbackTransaction();
            open_ = false;
        }
}

} // namespace hwd
} // namespace rpct
