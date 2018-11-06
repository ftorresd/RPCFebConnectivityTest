#ifndef rpct_hwd_Connection_h
#define rpct_hwd_Connection_h

#include "rpct/hwd/fwd.h"

namespace rpct {
namespace hwd {

class Connection
{
public:
    static Connection const default_;
public:
    Connection(integer_type _id
               , Device & _src
               , Connector const & _srconnector
               , Device & _dest
               , Connector const & _destconnector);
    Connection();

    integer_type getId() const;

    Device const & getSource() const;
    Device & getSource();
    Connector const & getSourceConnector() const;

    Device const & getDestination() const;
    Device & getDestination();
    Connector const & getDestinationConnector() const;

    bool operator< (Connection const & _connection) const;
    bool operator==(Connection const & _connection) const;
    bool operator!=(Connection const & _connection) const;

protected:
    integer_type id_;
    Device * src_;
    Connector const * srcconnector_;
    Device * dest_;
    Connector const * destconnector_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/Connection-inl.h"

#endif // rpct_hwd_Connection_h
