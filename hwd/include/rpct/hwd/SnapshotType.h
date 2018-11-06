#ifndef rpct_hwd_SnapshotType_h
#define rpct_hwd_SnapshotType_h

#include <string>

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class SnapshotType
{
public:
    static SnapshotType const default_;

public:
    SnapshotType(integer_type _id
                 , std::string const & _name);
    SnapshotType();

    integer_type getId() const;
    std::string const & getName() const;

    bool operator< (SnapshotType const & _snapshottype) const;
    bool operator==(SnapshotType const & _snapshottype) const;
    bool operator!=(SnapshotType const & _snapshottype) const;

protected:
    integer_type id_;
    std::string name_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/SnapshotType-inl.h"

#endif // rpct_hwd_SnapshotType_h
