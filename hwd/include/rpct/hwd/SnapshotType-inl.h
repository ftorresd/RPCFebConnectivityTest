#ifndef rpct_hwd_SnapshotType_inl_h
#define rpct_hwd_SnapshotType_inl_h

#include "rpct/hwd/SnapshotType.h"

namespace rpct {
namespace hwd {

inline integer_type SnapshotType::getId() const
{
    return id_;
}

inline std::string const & SnapshotType::getName() const
{
    return name_;
}

inline bool SnapshotType::operator<(SnapshotType const & _snapshottype) const
{
    return (id_ < _snapshottype.id_);
}

inline bool SnapshotType::operator==(SnapshotType const & _snapshottype) const
{
    return (id_ == _snapshottype.id_);
}

inline bool SnapshotType::operator!=(SnapshotType const & _snapshottype) const
{
    return (id_ != _snapshottype.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_SnapshotType_inl_h
