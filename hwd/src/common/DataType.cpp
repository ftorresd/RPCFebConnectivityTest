#include "rpct/hwd/DataType.h"

namespace rpct {
namespace hwd {

std::string const DataType::integer_name_ = std::string("integer");
std::string const DataType::float_name_   = std::string("float");
std::string const DataType::text_name_    = std::string("text");
std::string const DataType::blob_name_    = std::string("blob");

DataType const DataType::integer_ = DataType(integer_id_, integer_name_);
DataType const DataType::float_   = DataType(float_id_, float_name_);
DataType const DataType::text_    = DataType(text_id_, text_name_);
DataType const DataType::blob_    = DataType(blob_id_, blob_name_);

DataType const DataType::default_ = DataType();

DataType::DataType(integer_type _id
                   , std::string const & _name)
    : id_(_id)
    , name_(&_name)
{}

DataType::DataType()
    : id_(integer_id_)
    , name_(&integer_name_)
{}

} // namespace hwd
} // namespace rpct
