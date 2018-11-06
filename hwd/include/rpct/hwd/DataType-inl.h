#ifndef rpct_hwd_DataType_inl_h
#define rpct_hwd_DataType_inl_h

#include "rpct/hwd/DataType.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline integer_type DataType::getId() const
{
    return id_;
}

inline std::string const & DataType::getName() const
{
    if (name_)
        return *name_;
    throw ("Attempt to get Name of incomplete DataType");
}

inline bool DataType::operator<(DataType const & _datatype) const
{
    return (id_ < _datatype.id_);
}

inline bool DataType::operator==(DataType const & _datatype) const
{
    return (id_ == _datatype.id_);
}

inline bool DataType::operator!=(DataType const & _datatype) const
{
    return (id_ != _datatype.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_DataType_inl_h
