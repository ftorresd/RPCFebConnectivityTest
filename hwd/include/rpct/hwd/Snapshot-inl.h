#ifndef rpct_hwd_Snapshot_inl_h
#define rpct_hwd_Snapshot_inl_h

#include "rpct/hwd/Snapshot.h"

#include <stdexcept>

#include "rpct/hwd/SnapshotType.h"

namespace rpct {
namespace hwd {

inline System const & Snapshot::getSystem() const
{
    if (system_)
        return *system_;
    throw std::logic_error("Attempt to get System of an incomplete Snapshot");
}

inline integer_type Snapshot::getId() const
{
    return id_;
}

inline SnapshotType const & Snapshot::getSnapshotType() const
{
    if (snapshottype_)
        return *snapshottype_;
    throw std::logic_error("Attempt to get SnapshotType of an incomplete Snapshot");
}

inline rpct::tools::Time const & Snapshot::getTime() const
{
    return time_;
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Snapshot_inl_h
