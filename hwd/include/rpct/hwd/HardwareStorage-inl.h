#ifndef rpct_hwd_HardwareStorage_inl_h
#define rpct_hwd_HardwareStorage_inl_h

#include "rpct/hwd/HardwareStorage.h"

#include <sstream>
#include <stdexcept>

#include "rpct/hwd/DataType.h"

namespace rpct {
namespace hwd {

inline std::string const & HardwareStorage::getFileName() const
{
    return filename_;
}

inline DataType const & HardwareStorage::getDataType(integer_type _id) const
{
    switch (_id) {
    case DataType::integer_id_:
        return DataType::integer_;
        break;
    case DataType::float_id_:
        return DataType::float_;
        break;
    case DataType::text_id_:
        return DataType::text_;
        break;
    case DataType::blob_id_:
        return DataType::blob_;
        break;
    default:
        std::ostringstream _errormsg;
        _errormsg << "Could not find DataType " << _id << ".";
        throw std::logic_error(_errormsg.str());
        break;
    }
}

inline DataType const & HardwareStorage::getDataType(std::string const & _name) const
{
    if (_name == DataType::integer_name_)
        return DataType::integer_;
    else if (_name == DataType::float_name_)
        return DataType::float_;
    else if (_name == DataType::text_name_)
        return DataType::text_;
    else if (_name == DataType::blob_name_)
        return DataType::blob_;
    else
        throw std::logic_error(std::string("Could not find DataType ") + _name + ".");
}

template<typename t_cpp_value>
inline void HardwareStorage::publish(Device const & _device, Parameter const & _parameter, t_cpp_value const & _value
                                     , rpct::tools::Time const & _time)
{
    registerParameterValue(_device, _parameter
                           , storage_type<t_cpp_value>::get(_value)
                           , _time);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_HardwareStorage_inl_h
