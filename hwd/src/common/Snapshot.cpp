#include "rpct/hwd/Snapshot.h"

#include "rpct/hwd/SnapshotType.h"
#include "rpct/hwd/System.h"

namespace rpct {
namespace hwd {

Snapshot::Snapshot(System const & _system
                   , integer_type _id
                   , SnapshotType const & _snapshottype
                   , rpct::tools::Time const & _time)
    : system_(&_system)
    , id_(_id)
    , snapshottype_(&_snapshottype)
    , time_(_time)
{}

Snapshot::Snapshot(rpct::tools::Time const & _time
                   , integer_type _id)
    : system_(0)
    , id_(_id)
    , snapshottype_(&(SnapshotType::default_))
    , time_(_time)
{}

bool Snapshot::operator<(Snapshot const & _snapshot) const
{
    return (time_ < _snapshot.time_
            || (time_ == _snapshot.time_
                && id_ < _snapshot.id_)
            );
}

bool Snapshot::operator==(Snapshot const & _snapshot) const
{
    return (id_ == _snapshot.id_
            && time_ == _snapshot.time_);
}

bool Snapshot::operator!=(Snapshot const & _snapshot) const
{
    return (id_ != _snapshot.id_
            || time_ != _snapshot.time_);
}

} // namespace hwd
} // namespace rpct
