#ifndef rpct_hwd_fwd_h
#define rpct_hwd_fwd_h

#include <cstddef>
#include <string>
#include <vector>
#include <set>
#include <limits>

#include "rpct/hwd/DataType.h"

namespace rpct {
namespace hwd {

// directions
enum Direction {
    in_ = 0
    , out_ = 1
    , bidirectional_ = 2
};

class State;
class DeviceFlags;
class DeviceFlagsMask;
class ParameterType;
class Parameter;
class Service;
class Connector;
class DeviceType;
class Connection;
class Device;
class System;

class ParameterSetting;
class ParameterValue;
class DeviceConfiguration;
class Configuration;
class StandaloneDeviceConfiguration;

class DeviceObservables;
class Observables;

class SnapshotType;
class Snapshot;

class HardwareStorage;

template<typename t_cpp_type
         , bool t_is_specialized = std::numeric_limits<t_cpp_type>::is_specialized
         , bool t_is_integer = std::numeric_limits<t_cpp_type>::is_integer>
struct storage_type
{};

template<typename t_cpp_type>
struct storage_type<t_cpp_type, true, true>
{
    static DataType const & datatype();
    typedef t_cpp_type   cpp_type;
    typedef integer_type sqlite_type;
    static sqlite_type get(cpp_type const & _value);
    static void set(sqlite_type const & _in, cpp_type & _out);
};

template<typename t_cpp_type>
struct storage_type<t_cpp_type, true, false>
{
    static DataType const & datatype();
    typedef t_cpp_type cpp_type;
    typedef float_type sqlite_type;
    static sqlite_type get(cpp_type const & _value);
    static void set(sqlite_type const & _in, cpp_type & _out);
};

template<>
struct storage_type<std::string, false, false>
{
    static DataType const & datatype();
    typedef std::string cpp_type;
    typedef text_type   sqlite_type;
    static sqlite_type get(std::string const & _value);
    static void set(sqlite_type const & _in, std::string & _out);
};

template<typename t_cpp_type>
struct storage_type<t_cpp_type, false, false>
{
    static DataType const & datatype();
    typedef t_cpp_type cpp_type;
    typedef blob_type  sqlite_type;
    static sqlite_type get(cpp_type const & _value);
    static void set(sqlite_type const & _in, cpp_type & _out);
};

template<typename t_cpp_type>
struct storage_type<std::vector<t_cpp_type>, false, false>
{
    static DataType const & datatype();
    typedef std::vector<t_cpp_type> cpp_type;
    typedef blob_type               sqlite_type;
    static sqlite_type get(cpp_type const & _value);
    static void set(sqlite_type const & _in, cpp_type & _out);
};

template<typename t_cpp_type>
struct storage_type<std::set<t_cpp_type>, false, false>
{
    static DataType const & datatype();
    typedef std::set<t_cpp_type> cpp_type;
    typedef blob_type               sqlite_type;
    static sqlite_type get(cpp_type const & _value);
    static void set(sqlite_type const & _in, cpp_type & _out);
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/fwd-inl.h"

#endif // rpct_hwd_fwd_h
