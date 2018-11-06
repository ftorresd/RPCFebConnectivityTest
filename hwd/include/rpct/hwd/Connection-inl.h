#ifndef rpct_hwd_Connection_inl_h
#define rpct_hwd_Connection_inl_h

#include "rpct/hwd/Connection.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline integer_type Connection::getId() const
{
    return id_;
}

inline Device const & Connection::getSource() const
{
    if (src_)
        return *src_;
    else
        throw std::logic_error("Attempt to get Source of an incomplete Connection");
}

inline Device & Connection::getSource()
{
    if (src_)
        return *src_;
    else
        throw std::logic_error("Attempt to get Source of an incomplete Connection");
}

inline Connector const & Connection::getSourceConnector() const
{
    if (srcconnector_)
        return *srcconnector_;
    else
        throw std::logic_error("Attempt to get Source Connector of an incomplete Connection");
}

inline Device const & Connection::getDestination() const
{
    if (dest_)
        return *dest_;
    else
        throw std::logic_error("Attempt to get Destination of an incomplete Connection");
}

inline Device & Connection::getDestination()
{
    if (dest_)
        return *dest_;
    else
        throw std::logic_error("Attempt to get Destination of an incomplete Connection");
}

inline Connector const & Connection::getDestinationConnector() const
{
    if (destconnector_)
        return *destconnector_;
    else
        throw std::logic_error("Attempt to get Destination Connector of an incomplete Connection");
}

inline bool Connection::operator<(Connection const & _connection) const
{
    return (getId() < _connection.getId());
}

inline bool Connection::operator==(Connection const & _connection) const
{
    return (getId() == _connection.getId());
}

inline bool Connection::operator!=(Connection const & _connection) const
{
    return (getId() != _connection.getId());
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_Connection_inl_h
