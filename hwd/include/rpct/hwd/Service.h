#ifndef rpct_hwd_Service_h
#define rpct_hwd_Service_h

#include <string>

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class Service
{
public:
    static Service const default_;

public:
    Service(integer_type _id
            , std::string const & _name);
    Service();

    integer_type getId() const;
    std::string const & getName() const;

    bool operator< (Service const & _service) const;
    bool operator==(Service const & _service) const;
    bool operator!=(Service const & _service) const;

protected:
    integer_type id_;
    std::string name_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/Service-inl.h"

#endif // rpct_hwd_Service_h
