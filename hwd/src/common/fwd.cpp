#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

DataType const & storage_type<std::string, false, false>::datatype()
{
    return DataType::text_;
}

} // namespace hwd
} // namespace rpct
