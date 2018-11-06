#include "rpct/hwd/ParameterType.h"

#include "rpct/hwd/DataType.h"

namespace rpct {
namespace hwd {

ParameterType const ParameterType::default_ = ParameterType();

ParameterType::ParameterType(integer_type _id
                             , std::string const & _name
                             , DataType const & _datatype)
    : id_(_id)
    , name_(_name)
    , datatype_(&_datatype)
{}

ParameterType::ParameterType()
    : id_(-1)
    , name_("DefaultParameterType")
    , datatype_(&(DataType::integer_))
{}

} // namespace hwd
} // namespace rpct
