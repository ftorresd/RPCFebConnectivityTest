#ifndef rpct_hwd_DataType_h
#define rpct_hwd_DataType_h

#include <stdint.h>
#include <string>
#include <vector>

namespace rpct {
namespace hwd {

class DataType
{
public:
    typedef ::int64_t                  integer_type;
    typedef double                     float_type;
    typedef std::string                text_type;
    typedef std::vector<unsigned char> blob_type;

    static integer_type const integer_id_ = 0
        , float_id_ = 1
        , text_id_  = 2
        , blob_id_  = 3;

    static std::string const integer_name_
        , float_name_
        , text_name_
        , blob_name_;

    static DataType const integer_
        , float_
        , text_
        , blob_;

    static DataType const default_;

public:
    DataType(integer_type _id
             , std::string const & _name);
    DataType();

    integer_type getId() const;
    std::string const & getName() const;

    bool operator< (DataType const & _datatype) const;
    bool operator==(DataType const & _datatype) const;
    bool operator!=(DataType const & _datatype) const;

protected:
    integer_type id_;
    std::string const * name_;
};

typedef DataType::integer_type integer_type;
typedef DataType::float_type   float_type;
typedef DataType::text_type    text_type;
typedef DataType::blob_type    blob_type;

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/DataType-inl.h"

#endif // rpct_hwd_DataType_h
