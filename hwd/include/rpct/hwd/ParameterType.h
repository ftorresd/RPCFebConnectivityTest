#ifndef rpct_hwd_ParameterType_h
#define rpct_hwd_ParameterType_h

#include <string>

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class ParameterType
{
public:
    static ParameterType const default_;

public:
    ParameterType(integer_type _id
                  , std::string const & _name
                  , DataType const & _datatype);
    ParameterType();

    integer_type getId() const;
    std::string const & getName() const;
    DataType const & getDataType() const;

    bool operator< (ParameterType const & _parametertype) const;
    bool operator==(ParameterType const & _parametertype) const;
    bool operator!=(ParameterType const & _parametertype) const;

protected:
    integer_type id_;
    std::string name_;
    DataType const * datatype_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/ParameterType-inl.h"

#endif // rpct_hwd_ParameterType_h
