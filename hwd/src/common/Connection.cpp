#include "rpct/hwd/Connection.h"

#include "rpct/hwd/Device.h"
#include "rpct/hwd/Connector.h"

namespace rpct {
namespace hwd {

Connection const Connection::default_ = Connection();

Connection::Connection(integer_type _id
                       , Device & _src
                       , Connector const & _srcconnector
                       , Device & _dest
                       , Connector const & _destconnector)
    : id_(_id)
    , src_(&_src)
    , srcconnector_(&_srcconnector)
    , dest_(&_dest)
    , destconnector_(&_destconnector)
{}

Connection::Connection()
    : id_(-1)
    , src_(0)
    , srcconnector_(0)
    , dest_(0)
    , destconnector_(0)
{}

} // namespace hwd
} // namespace rpct
