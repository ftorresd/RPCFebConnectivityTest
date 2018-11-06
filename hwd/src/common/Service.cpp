#include "rpct/hwd/Service.h"

namespace rpct {
namespace hwd {

Service const Service::default_ = Service();

Service::Service(integer_type _id
                 , std::string const & _name)
    : id_(_id)
    , name_(_name)
{}

Service::Service()
    : id_(-1)
    , name_("DefaultService")
{}

} // namespace hwd
} // namespace rpct
