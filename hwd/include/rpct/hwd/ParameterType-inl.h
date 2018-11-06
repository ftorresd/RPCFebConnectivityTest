#ifndef rpct_hwd_ParameterType_inl_h
#define rpct_hwd_ParameterType_inl_h

#include "rpct/hwd/ParameterType.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline integer_type ParameterType::getId() const
{
    return id_;
}

inline std::string const & ParameterType::getName() const
{
    return name_;
}

inline DataType const & ParameterType::getDataType() const
{
    if (datatype_)
        return *datatype_;
    throw std::logic_error("Attempt to get DataType of incomplete ParameterType");
}

inline bool ParameterType::operator<(ParameterType const & _parametertype) const
{
    return (id_ < _parametertype.id_);
}

inline bool ParameterType::operator==(ParameterType const & _parametertype) const
{
    return (id_ == _parametertype.id_);
}

inline bool ParameterType::operator!=(ParameterType const & _parametertype) const
{
    return (id_ != _parametertype.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_ParameterType_inl_h
