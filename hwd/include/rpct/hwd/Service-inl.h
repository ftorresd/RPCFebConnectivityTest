#ifndef rpct_hwd_Service_inl_h
#define rpct_hwd_Service_inl_h

#include "rpct/hwd/Service.h"

namespace rpct {
namespace hwd {

inline integer_type Service::getId() const
{
    return id_;
}

inline std::string const & Service::getName() const
{
    return name_;
}

inline bool Service::operator<(Service const & _service) const
{
    return (id_ < _service.id_);
}

inline bool Service::operator==(Service const & _service) const
{
    return (id_ == _service.id_);
}

inline bool Service::operator!=(Service const & _service) const
{
    return (id_ != _service.id_);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Service_inl_h
