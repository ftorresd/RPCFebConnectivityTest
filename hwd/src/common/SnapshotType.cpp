#include "rpct/hwd/SnapshotType.h"

namespace rpct {
namespace hwd {

SnapshotType const SnapshotType::default_ = SnapshotType();

SnapshotType::SnapshotType(integer_type _id
                           , std::string const & _name)
    : id_(_id)
    , name_(_name)
{}

SnapshotType::SnapshotType()
    : id_(-1)
    , name_("DefaultSnapshotType")
{}

} // namespace hwd
} // namespace rpct
