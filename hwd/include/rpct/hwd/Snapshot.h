#ifndef rpct_hwd_Snapshot_h
#define rpct_hwd_Snapshot_h

#include "rpct/hwd/fwd.h"

#include "rpct/tools/Time.h"

namespace rpct {
namespace hwd {

class Snapshot
{
public:
    Snapshot(System const & _system
             , integer_type _id
             , SnapshotType const & _snapshottype
             , rpct::tools::Time const & _time = rpct::tools::Time());
    Snapshot(rpct::tools::Time const & _time = rpct::tools::Time()
             , integer_type _id = -1);

    System const & getSystem() const;
    integer_type getId() const;
    SnapshotType const & getSnapshotType() const;
    rpct::tools::Time const & getTime() const;

    bool operator< (Snapshot const & _snapshot) const;
    bool operator==(Snapshot const & _snapshot) const;
    bool operator!=(Snapshot const & _snapshot) const;

protected:
    System const * system_;
    integer_type id_;
    SnapshotType const * snapshottype_;
    rpct::tools::Time time_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/Snapshot-inl.h"

#endif // rpct_hwd_Snapshot_h
